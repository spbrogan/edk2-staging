# @file HostUnitTestCompiler_plugin.py
##
# Copyright (c) Microsoft Corporation. All rights reserved.
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

import logging
from edk2toollib.uefi.edk2.parsers.dsc_parser import DscParser
from edk2toolext.environment.plugintypes.ci_build_plugin import ICiBuildPlugin
from edk2toolext.environment.uefi_build import UefiBuilder
from edk2toolext import edk2_logging
import os
import re


class CompilerPlugin(ICiBuildPlugin):

    # gets the tests name
    def GetTestName(self, packagename, environment):
        target = environment.GetValue("TARGET")
        return ("Edk2 CI Compile " + target + " " + packagename, packagename + ".CompileCheck." + target)

    def IsTargetDependent(self):
        return True

    def __GetPkgDsc(self, rootpath):
        try:
            allEntries = os.listdir(rootpath)
            dscsFound = []
            for entry in allEntries:
                if entry.lower().endswith("ci.dsc"):
                    return(os.path.join(rootpath, entry))
        except Exception:
            logging.error("Unable to find ci.dsc for package:{0}".format(rootpath))

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
        self._env = environment
        
        AP = Edk2pathObj.GetAbsolutePathOnThisSytemFromEdk2RelativePath(packagename)
        #
        # only get ci.dsc files
        #
        
        APDSC = self.__GetPkgDsc(AP) # self.get_dsc_name_in_dir(AP)
        AP_Path = Edk2pathObj.GetEdk2RelativePathFromAbsolutePath(APDSC)
        if AP is None or AP_Path is None or not os.path.isfile(APDSC):
            tc.SetSkipped()
            tc.LogStdError("1 warning(s) in {0} Compile. ci.dsc not found.".format(packagename))
            return -1

        logging.info("Building {0}".format(AP_Path))
        self._env.SetValue("ACTIVE_PLATFORM", AP_Path, "Set in Compiler Plugin")

        # Parse DSC to check for SUPPORTED_ARCHITECTURES
        dp = DscParser()
        dp.SetBaseAbsPath(Edk2pathObj.WorkspacePath)
        dp.SetPackagePaths(Edk2pathObj.PackagePathList)
        dp.ParseFile(AP_Path)
        if "SUPPORTED_ARCHITECTURES" in dp.LocalVars:
            SUPPORTED_ARCHITECTURES = dp.LocalVars["SUPPORTED_ARCHITECTURES"].split('|')
            TARGET_ARCHITECTURES = environment.GetValue("TARGET_ARCH").split(' ')

            # Skip if there is no intersection between SUPPORTED_ARCHITECTURES and TARGET_ARCHITECTURES
            if len(set(SUPPORTED_ARCHITECTURES) & set(TARGET_ARCHITECTURES)) == 0:
                tc.SetSkipped()
                tc.LogStdError("No supported architecutres to build")
                return -1

        uefiBuilder = UefiBuilder()
        # do all the steps
        # WorkSpace, PackagesPath, PInHelper, PInManager
        ret = uefiBuilder.Go(Edk2pathObj.WorkspacePath, os.pathsep.join(Edk2pathObj.PackagePathList), PLMHelper, PLM)
        if ret != 0:  # failure:
            error_count = ""
            if output_stream is not None:
                # seek to the start of the output stream
                output_stream.seek(0, 0)
                problems = edk2_logging.scan_compiler_output(output_stream)
                error_count = " with {} errors/warnings".format(len(problems))
                for level, problem_msg in problems:
                    if level == logging.ERROR:
                        message = "Compile: Error: {0}".format(problem_msg)
                        tc.LogStdError(message)
                        logging.error(message)
                    elif level == logging.WARNING:
                        message = "Compile: Warning: {0}".format(problem_msg)
                        tc.LogStdError(message)
                        logging.warning(message)
                    else:
                        message = "Compiler is unhappy: {0}".format(problem_msg)
                        tc.LogStdError(message)
                        logging.warning(message)
            tc.SetFailed("Compile failed for {0}".format(packagename) + error_count, "Compile_FAILED")
            tc.LogStdError("{0} Compile failed with error code {1} ".format(AP_Path, ret))
            return 1

        else:
            tc.SetSuccess()
            return 0
