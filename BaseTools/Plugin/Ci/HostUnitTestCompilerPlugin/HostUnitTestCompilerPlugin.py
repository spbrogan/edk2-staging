# @file Compiler_plugin.py
# Simple Project Mu Build Plugin to support
# compiling code
##
# Copyright (c) Microsoft Corporation. All rights reserved.
# SPDX-License-Identifier: BSD-2-Clause-Patent
##

import logging
from edk2toollib.uefi.edk2.parsers.dsc_parser import DscParser
from edk2toolext.environment.plugintypes.ci_build_plugin import ICiBuildPlugin
from edk2toolext.environment.uefi_build import UefiBuilder
from edk2toolext import edk2_logging
from edk2toollib.utility_functions import GetHostInfo
import os
import re


class HostUnitTestCompilerPlugin(ICiBuildPlugin):

    # gets the tests name
    def GetTestName(self, packagename, environment):
        num,types = self.__GetHostUnitTestArch(environment)
        types = types.replace(" ", "_")

        return ("Host Unit Test Compile for " + packagename + " arch " + types,
                packagename + ".CompileAndRunCheck." + types)

    def IsTargetDependent(self):
        return False

    def __GetPkgDsc(self, rootpath):
        try:
            allEntries = os.listdir(rootpath)
            dscsFound = []
            for entry in allEntries:
                if entry.lower().endswith("hut.dsc"):
                    return(os.path.join(rootpath, entry))
        except Exception:
            logging.error("Unable to find DSC for package:{0}".format(rootpath))

        return None

    #
    # Find the intersection of application types that can run on this host
    # and the TARGET_ARCH being build in this request.
    #
    # return tuple with (number of UEFI arch types, space separated string)
    def __GetHostUnitTestArch(self, environment):
        requested = environment.GetValue("TARGET_ARCH").split(' ')
        host = []
        if GetHostInfo().arch == 'x86':
            #assume 64bit can handle 64 and 32
            #assume 32bit can only handle 32
            ## change once IA32 issues resolved host.append("IA32")
            if GetHostInfo().bit == '64':
                host.append("X64")        
        elif GetHostInfo().arch == 'ARM':
            if GetHostInfo().bit == '64':
                host.append("AARCH64")
            elif GetHostInfo().bit == '32':
                host.append("ARM")

        willrun = set(requested) & set(host)
        return (len(willrun), " ".join(willrun))
        

    ##
    # External function of plugin.  This function is used to perform the task of the ICiBuildPlugin Plugin
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
        environment.SetValue("CI_BUILD_TYPE", "host_unit_test", "Set in HostUnitTestCompilerPlugin")
        AP = Edk2pathObj.GetAbsolutePathOnThisSytemFromEdk2RelativePath(packagename)
        #
        # only get hut.dsc files
        #
        
        APDSC = self.__GetPkgDsc(AP) # self.get_dsc_name_in_dir(AP)
        AP_Path = Edk2pathObj.GetEdk2RelativePathFromAbsolutePath(APDSC)
        if AP is None or AP_Path is None or not os.path.isfile(APDSC):
            tc.SetSkipped()
            tc.LogStdError("1 warning(s) in {0} Compile. hut.DSC not found.".format(packagename))
            return -1

        logging.info("Building {0}".format(AP_Path))
        self._env.SetValue("ACTIVE_PLATFORM", AP_Path, "Set in Compiler Plugin")
        num, RUNNABLE_ARCHITECTURES = self.__GetHostUnitTestArch(environment)
        if(num == 0):
            tc.SetSkipped()
            tc.LogStdError("No host architecture compatibility")
            return -1

        if not environment.SetValue("TARGET_ARCH",
                                    RUNNABLE_ARCHITECTURES,
                                    "Update Target Arch based on Host Support"):
            #use AllowOverride function since this is a controlled attempt to change
            environment.AllowOverride("TARGET_ARCH")
            if not environment.SetValue("TARGET_ARCH",
                                        RUNNABLE_ARCHITECTURES,
                                        "Update Target Arch based on Host Support"):
                raise RuntimeError("Can't Change TARGET_ARCH as required")

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
                tc.LogStdError("No supported architecutres to build for host unit tests")
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
