## @file WinHostBasedVcVars.py
# Plugin to update the Host-Based build environment with the shell_env content from VcVars.
##
# Copyright (c) Microsoft Corporation
#
##
import os
from edk2toolext.environment.plugintypes.uefi_build_plugin import IUefiBuildPlugin
import edk2toollib.windows.locate_tools as locate_tools
from edk2toolext.environment import shell_environment

class WinHostBasedVcVars(IUefiBuildPlugin):

    def do_post_build(self, thebuilder):
        return 0

    def do_pre_build(self, thebuilder):
        ci_type = thebuilder.env.GetValue('CI_BUILD_TYPE')
        if ci_type == 'host_unit_test':
            # Use the tools lib to determine the correct values for the vars that interest us.
            interesting_keys = ["ExtensionSdkDir", "INCLUDE", "LIB", "LIBPATH", "UniversalCRTSdkDir",
                                "UCRTVersion", "WindowsLibPath", "WindowsSdkBinPath", "WindowsSdkDir", "WindowsSdkVerBinPath",
                                "WindowsSDKVersion","VCToolsInstallDir"]
            vs_vars = locate_tools.QueryVcVariables(interesting_keys, "amd64")
            for (k,v) in vs_vars.items():
                shell_environment.GetEnvironment().set_shell_var(k, v)

        return 0
