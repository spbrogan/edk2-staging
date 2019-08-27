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
        """ Find all duplicate guids by name and report them as errors
        """
        # Sort the list by guid
        namesorted = sorted(guidlist, key=lambda x: x.name.upper())

        previous = None  # Store previous entry for comparison
        error = None
        errors = []
        for index in range(len(namesorted)):
            i = namesorted[index]
            if(previous is not None):
                if i.name == previous.name:  # Error
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
        # Need to get more ignore patters from config

        # Parse the workspace for all GUIDs
        gs = GuidList.guidlist_from_filesystem(
            Edk2pathObj.WorkspacePath, ignore_lines=All_Ignores)

        # Remove ignored guidnames

        # Remove ignored guidvalues

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
