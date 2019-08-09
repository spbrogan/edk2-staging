## @file HostBasedUnitTestRunner.py
# Plugin to located any host-based unit tests in the output directory and execute them.
##
# Copyright (c) Microsoft Corporation
#
##
import os
from edk2toolext.environment.plugintypes.uefi_build_plugin import IUefiBuildPlugin
from edk2toolext.environment import shell_environment
from edk2toollib.utility_functions import RunCmd

class HostBasedUnitTestRunner(IUefiBuildPlugin):

    def do_post_build(self, thebuilder):
        shell_env = shell_environment.GetEnvironment()

        logging.log(MuLogging.get_section_level(), "Run Host based Unit Tests")
        path = thebuilder.env.GetValue("BUILD_OUTPUT_BASE")
        for arch in thebuilder.env.GetValue("TARGET_ARCH").split():
            logging.log( MuLogging.get_subsection_level(), "Testing for architecture: " + arch)
            cp = os.path.join(path, arch)

            # If any old results XML files exist, clean them up.
            for old_result in glob.iglob(os.path.join(cp, "*.result.xml")):
                os.remove(old_result)

            # Determine whether any tests exist.
            testList = glob.glob(os.path.join(cp, "*Test*.exe"))
            for test in testList:
                # Configure output name.
                shell_env.set_shell_env('CMOCKA_XML_FILE', test + ".%g." + arch + ".result.xml")

                # Run the test.
                ret = RunCmd('"' + test + '"', "", workingdir=cp)
                if(ret != 0):
                    logging.error("UnitTest Execution Error: " + os.path.basename(test))
                else:
                    logging.info("UnitTest Completed: " + os.path.basename(test))
                    file_match_pattern = test + ".*." + arch + ".result.xml"
                    xml_results_list = glob.glob(file_match_pattern)
                    for xml_result_file in xml_results_list:
                        root = xml.etree.ElementTree.parse(xml_result_file).getroot()
                        for suite in root:
                            for case in suite:
                                for result in case:
                                    if result.tag == 'failure':
                                        logging.warning("%s Test Failed" % os.path.basename(test))
                                        logging.warning("  %s - %s" % (case.attrib['name'], result.text))

        return 0

    def do_pre_build(self, thebuilder):
        return 0
