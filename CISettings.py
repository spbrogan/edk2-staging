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
        self.ActualPackages = []
        self.ActualTargets = []
        self.ActualArchitectures = []
        self.ActualToolChainTag = ""

    # ####################################################################################### #
    #                             Extra CmdLine configuration                                 #
    # ####################################################################################### #

    def AddCommandLineOptions(self, parserObj):
        pass

    def RetrieveCommandLineOptions(self, args):
        pass

    # ####################################################################################### #
    #                        Default Support for this Ci Build                                #
    # ####################################################################################### #

    def GetPackagesSupported(self):
        ''' return iterable of edk2 packages supported by this build. 
        These should be edk2 workspace relative paths '''

        return ("MdeModulePkg",
                "MdePkg",
                "NetworkPkg",
                "PcAtChipsetPkg",
                "SecurityPkg",
                "UefiCpuPkg",
                "FmpDevicePkg",
                "ShellPkg",
                "FatPkg",
                "CryptoPkg")

    def GetArchitecturesSupported(self):
        ''' return iterable of edk2 architectures supported by this build '''
        return ("IA32",
                "X64",
                "AARCH64")

    def GetTargetsSupported(self):
        ''' return iterable of edk2 target tags supported by this build '''
        return ("DEBUG", "RELEASE", "NO-TARGET", "NOOPT")

    # ####################################################################################### #
    #                     Verify and Save requested Ci Build Config                           #
    # ####################################################################################### #

    def SetPackages(self, list_of_requested_packages):
        ''' Confirm the requested package list is valid and configure SettingsManager
        to build the requested packages.

        Raise UnsupportedException if a requested_package is not supported
        '''
        unsupported = set(list_of_requested_packages) - \
            set(self.GetPackagesSupported())
        if(len(unsupported) > 0):
            logging.critical(
                "Unsupported Package Requested: " + " ".join(unsupported))
            raise Exception("Unsupported Package Requested: " +
                            " ".join(unsupported))
        self.ActualPackages = list_of_requested_packages

    def SetArchitectures(self, list_of_requested_architectures):
        ''' Confirm the requests architecture list is valid and configure SettingsManager
        to run only the requested architectures.

        Raise Exception if a list_of_requested_architectures is not supported
        '''
        unsupported = set(list_of_requested_architectures) - \
            set(self.GetArchitecturesSupported())
        if(len(unsupported) > 0):
            logging.critical(
                "Unsupported Architecture Requested: " + " ".join(unsupported))
            raise Exception(
                "Unsupported Architecture Requested: " + " ".join(unsupported))
        self.ActualArchitectures = list_of_requested_architectures

    def SetTargets(self, list_of_requested_target):
        ''' Confirm the request target list is valid and configure SettingsManager
        to run only the requested targets.

        Raise UnsupportedException if a requested_target is not supported
        '''
        unsupported = set(list_of_requested_target) - \
            set(self.GetTargetsSupported())
        if(len(unsupported) > 0):
            logging.critical(
                "Unsupported Targets Requested: " + " ".join(unsupported))
            raise Exception("Unsupported Targets Requested: " +
                            " ".join(unsupported))
        self.ActualTargets = list_of_requested_target

    # ####################################################################################### #
    #                         Actual Configuration for Ci Build                               #
    # ####################################################################################### #

    def GetActiveScopes(self):
        ''' get scope '''
        scopes = ("corebuild",)

        self.ActualToolChainTag = shell_environment.GetBuildVars().GetValue("TOOL_CHAIN_TAG", "")

        if (GetHostInfo().os == "Linux"
            and "AARCH64" in self.ActualArchitectures and
                self.ActualToolChainTag.upper().startswith("GCC")):

            scopes += ("gcc_aarch64_linux",)

        if GetHostInfo().os == "Windows":
            scopes += ("host-test-win",)

        return scopes

    def GetRequiredRepos(self):
        rr = ("ArmPkg/Library/ArmSoftFloatLib/berkeley-softfloat-3",
              "CmockaHostUnitTestPkg/Library/CmockaLib/cmocka")
        #NeedsOpenSSL = {"CryptoPkg", "SecurityPkg", "NetworkPkg"}
        #if (len(NeedsOpenSSL - set(self.ActualPackages)) != len(NeedsOpenSSL)):
        rr += ("CryptoPkg/Library/OpensslLib/openssl",)
        return rr

    def GetName(self):
        return "Edk2"

    def GetOmnicachePath(self):
        return os.environ.get('OMNICACHE_PATH')

    def GetDependencies(self):
        return []

    def GetPackagesPath(self):
        return ()

    def GetWorkspaceRoot(self):
        ''' get WorkspacePath '''
        return os.path.dirname(os.path.abspath(__file__))
