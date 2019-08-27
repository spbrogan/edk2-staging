# @file LibraryClassCheck.py
# Simple CI Build Plugin to support
# checking all of the libraries in include/library folder are listed in library class DEC file
#
# Making sure all public includes are fully declared in DECs
##
# Copyright (c) Microsoft Corporation. All rights reserved.
# SPDX-License-Identifier: BSD-2-Clause-Patent
##
import logging
import os
from edk2toolext.environment.plugintypes.ci_build_plugin import ICiBuildPlugin
from edk2toollib.uefi.edk2.parsers.dec_parser import DecParser
from edk2toollib.uefi.edk2.parsers.inf_parser import InfParser


class LibraryClassCheck(ICiBuildPlugin):

    def GetTestName(self, packagename, environment):
        return ("LibraryClassCheck " + packagename, "CI.LibraryClassCheck." + packagename)

    def __GetPkgDec(self, rootpath):
        try:
            allEntries = os.listdir(rootpath)
            for entry in allEntries:
                if entry.lower().endswith(".dec"):
                    return(os.path.join(rootpath, entry))
        except Exception:
            logging.error("Unable to find DEC for package:{0}".format(rootpath))

        return None

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
        overall_status = 0

        abs_pkg_path = Edk2pathObj.GetAbsolutePathOnThisSytemFromEdk2RelativePath(packagename)
        abs_dec_path = self.__GetPkgDec(abs_pkg_path)
        wsr_dec_path = Edk2pathObj.GetEdk2RelativePathFromAbsolutePath(abs_dec_path)

        if abs_dec_path is None or wsr_dec_path is "" or not os.path.isfile(abs_dec_path):
            tc.SetSkipped()
            tc.LogStdError("No DEC file {0} in package {1}".format(abs_dec_path, abs_pkg_path))
            return -1

        ## Get all header files in include/library
        AbsLibraryIncludePath = os.path.join(abs_pkg_path, "Include", "Library") # this is fragile
        if(not os.path.isdir(AbsLibraryIncludePath)):
            tc.SetSkipped()
            tc.LogStdError(f"No known public header folder for library files {AbsLibraryIncludePath}")
            return -1

        hfiles = self.WalkDirectoryForExtension([".h"], AbsLibraryIncludePath)
        hfiles = [os.path.relpath(x,abs_pkg_path) for x in hfiles]  # make package root relative path
        hfiles = [x.replace("\\", "/") for x in hfiles]  # make package relative path

        dec = DecParser()
        dec.SetBaseAbsPath(Edk2pathObj.WorkspacePath).SetPackagePaths(Edk2pathObj.PackagePathList)
        dec.ParseFile(wsr_dec_path)

        ## Attempt to find library classes
        for lcd in dec.LibraryClasses:
            logging.debug(f"Looking for Library Class {lcd.path}")
            try:
                hfiles.remove(lcd.path)

            except ValueError:
                tc.LogStdError(f"Library {lcd.name} with path {lcd.path} not found in package filesystem")
                logging.error(f"Library {lcd.name} with path {lcd.path} not found in package filesystem")
                overall_status += + 1

        ## any remaining hfiles are not described in DEC
        for h in hfiles:
            tc.LogStdError(f"Library Header File {h} not declared in package DEC {wsr_dec_path}")
            logging.error(f"Library Header File {h} not declared in package DEC {wsr_dec_path}")
            overall_status += 1


        # If XML object exists, add result
        if overall_status is not 0:
            tc.SetFailed("LibraryClassCheck {0} Failed.  Errors {1}".format(wsr_dec_path, overall_status), "CHECK_FAILED")
        else:
            tc.SetSuccess()
        return overall_status
