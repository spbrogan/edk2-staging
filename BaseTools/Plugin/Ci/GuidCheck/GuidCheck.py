# @file GuidCheck.py
# Simple CI Build Plugin to support
# checking all of the guids to make sure they are unique
#
##
# Copyright (c) Microsoft Corporation. All rights reserved.
# SPDX-License-Identifier: BSD-2-Clause-Patent
##
import logging
from edk2toolext.environment.plugintypes.ci_build_plugin import ICiBuildPlugin
from edk2toollib.uefi.edk2.parsers.guid_list import GuidList


class GuidCheck(ICiBuildPlugin):
    """
    A CiBuildPlugin that scans the code tree and looks for duplicate guids
    from the package being tested.  

    Configuration options:
    "GuidCheck": {
        "IgnoreGuidName": [],
        "IgnoreGuidValue": [],
        "IgnoreFoldersAndFiles: []
    }
    """

    def GetTestName(self, packagename, environment):
        return ("GuidClassCheck " + packagename, "CI.GuidCheck." + packagename)

    def _FindConflictingGuidValues(self, guidlist: list) -> list:
        """ Find all duplicate guids by guid value and report them as errors
        """
        # Sort the list by guid
        guidsorted = sorted(
            guidlist, key=lambda x: x.guid.upper(), reverse=True)

        previous = None  # Store previous entry for comparison
        error = None
        errors = []
        for index in range(len(guidsorted)):
            i = guidsorted[index]
            if(previous is not None):
                if i.guid == previous.guid:  # Error
                    if(error is None):
                        # Catch errors with more than 1 conflict
                        error = ErrorEntry("guid")
                        error.entries.append(previous)
                        errors.append(error)
                    error.entries.append(i)
                else:
                    # no match.  clear error
                    error = None
            previous = i
        return errors

    def _FindConflictingGuidNames(self, guidlist: list) -> list:
        """ Find all duplicate guids by name and if they are not all
        from inf files report them as errors.  It is ok to have 
        BASE_NAME duplication.  

        Is this useful?  It would catch two same named guids in dec file
        that resolve to different values. 
        """
        # Sort the list by guid
        namesorted = sorted(guidlist, key=lambda x: x.name.upper())

        previous = None  # Store previous entry for comparison
        error = None
        errors = []
        for index in range(len(namesorted)):
            i = namesorted[index]
            if(previous is not None):
                # If name matches
                if i.name == previous.name:
                    if(error is None):
                        # Catch errors with more than 1 conflict
                        error = ErrorEntry("name")
                        error.entries.append(previous)
                        errors.append(error)
                    error.entries.append(i)
                else:
                    # no match.  clear error
                    error = None
            previous = i

            # Loop thru and remove any errors where all files are infs as it is ok if 
            # they have the same inf base name.  
            for e in errors[:]:
                if len( [entries for en in e.entries if not en.absfilepath.lower().endswith(".inf")]) == 0:
                    errors.remove(e)

        return errors

    ##
    # External function of plugin.  This function is used to perform the task of the MuBuild Plugin
    #
    #   - package is the edk2 path to package.  This means workspace/packagepath relative.
    #   - edk2path object configured with workspace and packages path
    #   - PkgConfig Object (dict) for the pkg
    #   - EnvConfig Object
    #   - Plugin Manager Instance
    #   - Plugin Helper Obj Instance
    #   - Junit Logger
    #   - output_stream the StringIO output stream from this plugin via logging

    def RunBuildPlugin(self, packagename, Edk2pathObj, pkgconfig, environment, PLM, PLMHelper, tc, output_stream=None):
        Errors = []

        abs_pkg_path = Edk2pathObj.GetAbsolutePathOnThisSytemFromEdk2RelativePath(
            packagename)

        if abs_pkg_path is None:
            tc.SetSkipped()
            tc.LogStdError("No package {0}".format(packagename))
            return -1

        All_Ignores = [".pyc", "/Build", "/Conf"]
        # Parse the config for other ignores
        if "IgnoreFoldersAndFiles" in pkgconfig:
            All_Ignores.extend(pkgconfig["IgnoreFoldersAndFiles"])

        # Parse the workspace for all GUIDs
        gs = GuidList.guidlist_from_filesystem(
            Edk2pathObj.WorkspacePath, ignore_lines=All_Ignores)

        # Remove ignored guidvalue
        if "IgnoreGuidValue" in pkgconfig:
            for a in pkgconfig["IgnoreGuidValue"]:
                try:
                    tc.LogStdOut("Ignoring Guid {0}".format(a.upper()))
                    for b in gs[:]:
                        if b.guid == a.upper():
                            gs.remove(b)
                except:
                    tc.LogStdError("GuidCheck.IgnoreGuid -> {0} not found.  Invalid ignore guid".format(a.upper()))
                    logging.info("GuidCheck.IgnoreGuid -> {0} not found.  Invalid ignore guid".format(a.upper()))

        # Remove ignored guidname
        if "IgnoreGuidName" in pkgconfig:
            for a in pkgconfig["IgnoreGuidName"]:
                try:
                    tc.LogStdOut("Ignoring Guid {0}".format(a))
                    for b in gs[:]:
                        if b.name == a:
                            gs.remove(b)
                except:
                    tc.LogStdError("GuidCheck.IgnoreGuidName -> {0} not found.  Invalid ignore guid".format(a))
                    logging.info("GuidCheck.IgnoreGuidName -> {0} not found.  Invalid ignore guid".format(a))

        # Find conflicting Guid Values
        Errors.extend(self._FindConflictingGuidValues(gs))

        # Find conflicting Guid Names
        Errors.extend(self._FindConflictingGuidNames(gs))

        # Log errors for anything within the package under test
        for er in Errors[:]:
            InMyPackage = False
            for a in er.entries:
                if abs_pkg_path in a.absfilepath:
                    InMyPackage = True
                    break
            if(not InMyPackage):
                Errors.remove(er)
            else:
                logging.error(str(er))
                tc.LogStdError(str(er))

        # add result to test case
        overall_status = len(Errors)
        if overall_status is not 0:
            tc.SetFailed("GuidCheck {0} Failed.  Errors {1}".format(
                packagename, overall_status), "CHECK_FAILED")
        else:
            tc.SetSuccess()
        return overall_status


class ErrorEntry():
    """ Custom/private class for reporting errors in the GuidList
    """

    def __init__(self, errortype):
        self.type = errortype  # 'guid' or 'name' depending on error type
        self.entries = []  # GuidListEntry that are in error condition

    def __str__(self):
        a = f"Error Duplicate {self.type}: "
        if(self.type == "guid"):
            a += f" {self.entries[0].guid}"
        elif(self.type == "name"):
            a += f" {self.entries[0].name}"

        a += f" ({len(self.entries)})\n"

        for e in self.entries:
            a += "\t" + str(e) + "\n"
        return a
