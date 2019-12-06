# @file HostBasedUnitTestRunner.py
#
# Plugin to support building and executing modules of type HOST-APPLICATION.
# This module type is used for writing Host Based Unit Tests on a Microsoft Windows Host.
#
##
# Copyright (c) Microsoft Corporation.
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
import os
import logging
import glob
import xml.etree.ElementTree
from edk2toolext.environment.plugintypes.uefi_build_plugin import IUefiBuildPlugin
from edk2toolext import edk2_logging
import edk2toollib.windows.locate_tools as locate_tools
from edk2toolext.environment import shell_environment
from edk2toollib.utility_functions import RunCmd


class HostBasedUnitTestRunner(IUefiBuildPlugin):

    FILE_NAME_GLOB_PATTERN_FOR_TEST_EXECUTABLES = "*Test*.exe"
    FILE_NAME_EXT_FOR_TEST_RESULT = ".result.xml"

    SUPPORTED_TOOL_CHAIN_TAGS = ("VS2017", "VS2019")

    def _check_if_should_run(self, thebuilder):
        '''
        determine if the build environment has been configured to build and execute
        host based unit tests

        RETURNS:
            True == Configured to run
            False = Not configured to run
        '''
        if not thebuilder.env.GetValue('CI_BUILD_TYPE') == 'host_unit_test':
            return False

        if thebuilder.env.GetValue("TOOL_CHAIN_TAG") not in self.SUPPORTED_TOOL_CHAIN_TAGS:
            return False

        return True

    def do_pre_build(self, thebuilder):
        '''
        Works with the compiler (either the HostBasedCompilerPlugin or an other Builder) to set
        up the environment that will be needed to build host-based unit tests.

        EXPECTS:
        - Build Var 'CI_BUILD_TYPE' - If not set to 'host_unit_test', will not do anything.

        UPDATES:
        - Shell Var (Several) - Updates the shell with all vars listed in interesting_keys.
        - Shell Path - Updated from QueryVcVariables()
        - Shell Var 'CMOCKA_MESSAGE_OUTPUT'
        '''
        if not self._check_if_should_run(thebuilder):
            return 0

        shell_env = shell_environment.GetEnvironment()
        # Use the tools lib to determine the correct values for the vars that interest us.
        interesting_keys = ["ExtensionSdkDir", "INCLUDE", "LIB", "LIBPATH", "UniversalCRTSdkDir",
                            "UCRTVersion", "WindowsLibPath", "WindowsSdkBinPath", "WindowsSdkDir", "WindowsSdkVerBinPath",
                            "WindowsSDKVersion", "VCToolsInstallDir"]

        # set the vs_version so the libs and sdk align with the toolchain being used
        vs_vars = locate_tools.QueryVcVariables(interesting_keys, "amd64", vs_version=thebuilder.env.GetValue("TOOL_CHAIN_TAG").lower())
        for (k, v) in vs_vars.items():
            shell_env.set_shell_var(k, v)
        return 0

    def do_post_build(self, thebuilder):
        '''
        After a build, will automatically locate and run all host-based unit tests. Logs any
        failures with Warning severity and will return a count of the failures as the return code.

        EXPECTS:
        - Build Var 'CI_BUILD_TYPE' - If not set to 'host_unit_test', will not do anything.

        UPDATES:
        - Shell Var 'CMOCKA_XML_FILE'
        '''
        if not self._check_if_should_run(thebuilder):
            return 0

        shell_env = shell_environment.GetEnvironment()

        # Set up the reporting type for Cmocka.
        shell_env.set_shell_var('CMOCKA_MESSAGE_OUTPUT', 'xml')

        logging.log(edk2_logging.get_section_level(),
                    "Run Host based Unit Tests")
        path = thebuilder.env.GetValue("BUILD_OUTPUT_BASE")

        failure_count = 0

        for arch in thebuilder.env.GetValue("TARGET_ARCH").split():
            logging.log(edk2_logging.get_subsection_level(),
                        "Testing for architecture: " + arch)
            cp = os.path.join(path, arch)

            # If any old results XML files exist, clean them up.
            for old_result in glob.iglob(os.path.join(cp, "*" + self.FILE_NAME_EXT_FOR_TEST_RESULT)):
                os.remove(old_result)

            # Determine whether any tests exist.
            testList = glob.glob(os.path.join(cp, self.FILE_NAME_GLOB_PATTERN_FOR_TEST_EXECUTABLES))
            for test in testList:
                # Configure output name.
                shell_env.set_shell_var(
                    'CMOCKA_XML_FILE', test + ".%g." + arch + self.FILE_NAME_EXT_FOR_TEST_RESULT)

                # Run the test.
                ret = RunCmd('"' + test + '"', "", workingdir=cp)
                if(ret != 0):
                    logging.error("UnitTest Execution Error: " +
                                  os.path.basename(test))
                else:
                    logging.info("UnitTest Completed: " +
                                 os.path.basename(test))
                    file_match_pattern = test + ".*." + arch + self.FILE_NAME_EXT_FOR_TEST_RESULT
                    xml_results_list = glob.glob(file_match_pattern)
                    for xml_result_file in xml_results_list:
                        root = xml.etree.ElementTree.parse(
                            xml_result_file).getroot()
                        for suite in root:
                            for case in suite:
                                for result in case:
                                    if result.tag == 'failure':
                                        logging.warning(
                                            "%s Test Failed" % os.path.basename(test))
                                        logging.warning(
                                            "  %s - %s" % (case.attrib['name'], result.text))
                                        failure_count += 1
        # Clean up
        shell_env.set_shell_var('CMOCKA_XML_FILE', '')
        shell_env.set_shell_var('CMOCKA_MESSAGE_OUTPUT', '')


        return failure_count
