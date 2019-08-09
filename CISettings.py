# @file
#
# Copyright (c) 2018, Microsoft Corporation
# SPDX-License-Identifier: BSD-2-Clause-Patent
##
import os
from edk2toolext.environment import shell_environment
from edk2toolext.invocables.edk2_ci_build import CiBuildSettingsManager
from edk2toolext.invocables.edk2_setup import SetupSettingsManager
from edk2toolext.invocables.edk2_update import UpdateSettingsManager
from edk2toollib.utility_functions import GetHostInfo

class Settings(CiBuildSettingsManager, UpdateSettingsManager, SetupSettingsManager):

    def __init__(self):
        # plugin_skip_list = ["DependencyCheck", "CompilerPlugin"]
        # env = shell_environment.GetBuildVars()
        # for plugin in plugin_skip_list:
        #     env.SetValue(plugin.upper(), "skip", "set from settings file")
        shell_environment.GetBuildVars().SetValue("DependencyCheck", "skip", "hardcoded off in Settings until fixed")
        pass

    def AddCommandLineOptions(self, parserObj):
        parserObj.add_argument('--Tool_Chain', dest='tool_chain_tag', type=str, help='tool chain tag to use for this build')

    def RetrieveCommandLineOptions(self, args):
        if args.tool_chain_tag is not None:
            shell_environment.GetBuildVars().SetValue("TOOL_CHAIN_TAG", args.tool_chain_tag, "Set as cli parameter")
        # cache this so usage within CISettings is consistant. 
        self.ToolChainTagCacheValue = args.tool_chain_tag

    def GetActiveScopes(self):
        ''' get scope '''
        scopes = ("corebuild", "project_mu")

        if (GetHostInfo().os == "Linux"
            and "AARCH64" in self.GetArchSupported() and
            self.ToolChainTagCacheValue is not None and
            self.ToolChainTagCacheValue.upper().startswith("GCC")):
            
            scopes += ("gcc_aarch64_linux",)

        if GetHostInfo().os == "Windows":
            scopes += ("host-test-win",)

        return scopes

    def GetName(self):
        return "Basecore"

    def GetDependencies(self):
        return []

    def GetRequiredRepos(self):
        return ("CryptoPkg/Library/OpensslLib/openssl","ArmPkg/Library/ArmSoftFloatLib/berkeley-softfloat-3", "CmockaHostUnitTestPkg/Library/CmockaLib/cmocka")

    def GetPackages(self):
        return ("MdeModulePkg",
            "MdePkg",
            "NetworkPkg",
            "PcAtChipsetPkg",
            "SecurityPkg",
            "UefiCpuPkg")

    def GetPackagesPath(self):
        return ()

    def GetArchSupported(self):
        return ("IA32",
                "X64",
                "AARCH64")

    def GetTargetsSupported(self):
        return ("DEBUG", "RELEASE")

    def GetWorkspaceRoot(self):
        ''' get WorkspacePath '''
        return os.path.dirname(os.path.abspath(__file__))
