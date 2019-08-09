## @file HostBasedUnitTestRunner.py
# Plugin to located any host-based unit tests in the output directory and execute them.
##
# Copyright (c) Microsoft Corporation
#
##
import os
from edk2toolext.environment.plugintypes.uefi_build_plugin import IUefiBuildPlugin

class HostBasedUnitTestRunner(IUefiBuildPlugin):

    def do_post_build(self, thebuilder):
        # TODO: Some stuff here.
        return 0

    def do_pre_build(self, thebuilder):
        return 0
