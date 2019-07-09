## @ GenTestCaseTemplate.py
#
# Copyright (c) 2017, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent.
#
##

import os
import re
import sys
import time
import shutil

## Regular expression used to find out place holders in string template
gPlaceholderPattern = re.compile("\$\{([^$()\s]+)\}", re.MULTILINE | re.UNICODE)

class TemplateString(object):
    _REPEAT_START_FLAG = "BEGIN"
    _REPEAT_END_FLAG = "END"

    class Section(object):
        _LIST_TYPES = [type([]), type(set()), type((0,))]

        def __init__(self, TemplateSection, PlaceHolderList):
            self._Template = TemplateSection
            self._PlaceHolderList = []

            # Split the section into sub-sections according to the position of placeholders
            if PlaceHolderList:
                self._SubSectionList = []
                SubSectionStart = 0
                #
                # The placeholders passed in must be in the format of
                #
                #   PlaceHolderName, PlaceHolderStartPoint, PlaceHolderEndPoint
                #
                for PlaceHolder, Start, End in PlaceHolderList:
                    self._SubSectionList.append(TemplateSection[SubSectionStart:Start])
                    self._SubSectionList.append(TemplateSection[Start:End])
                    self._PlaceHolderList.append(PlaceHolder)
                    SubSectionStart = End
                if SubSectionStart < len(TemplateSection):
                    self._SubSectionList.append(TemplateSection[SubSectionStart:])
            else:
                self._SubSectionList = [TemplateSection]

        def __str__(self):
            return self._Template + " : " + str(self._PlaceHolderList)

        def Instantiate(self, PlaceHolderValues):
            RepeatTime = -1
            RepeatPlaceHolders = {}
            NonRepeatPlaceHolders = {}

            for PlaceHolder in self._PlaceHolderList:
                if PlaceHolder not in PlaceHolderValues:
                    continue
                Value = PlaceHolderValues[PlaceHolder]
                if type(Value) in self._LIST_TYPES:
                    if RepeatTime < 0:
                        RepeatTime = len(Value)
                    elif RepeatTime != len(Value):
                        EdkLogger.error(
                                    "TemplateString",
                                    PARAMETER_INVALID,
                                    "${%s} has different repeat time from others!" % PlaceHolder,
                                    ExtraData=str(self._Template)
                                    )
                    RepeatPlaceHolders["${%s}" % PlaceHolder] = Value
                else:
                    NonRepeatPlaceHolders["${%s}" % PlaceHolder] = Value

            if NonRepeatPlaceHolders:
                StringList = []
                for S in self._SubSectionList:
                    if S not in NonRepeatPlaceHolders:
                        StringList.append(S)
                    else:
                        StringList.append(str(NonRepeatPlaceHolders[S]))
            else:
                StringList = self._SubSectionList

            if RepeatPlaceHolders:
                TempStringList = []
                for Index in range(RepeatTime):
                    for S in StringList:
                        if S not in RepeatPlaceHolders:
                            TempStringList.append(S)
                        else:
                            TempStringList.append(str(RepeatPlaceHolders[S][Index]))
                StringList = TempStringList

            return "".join(StringList)

    ## Constructor
    def __init__(self, Template=None):
        self.String = ''
        self.IsBinary = False
        self._Template = Template
        self._TemplateSectionList = self._Parse(Template)

    ## str() operator
    #
    #   @retval     string  The string replaced
    #
    def __str__(self):
        return self.String

    ## Split the template string into fragments per the ${BEGIN} and ${END} flags
    #
    #   @retval     list    A list of TemplateString.Section objects
    #
    def _Parse(self, Template):
        SectionStart = 0
        SearchFrom = 0
        MatchEnd = 0
        PlaceHolderList = []
        TemplateSectionList = []
        while Template:
            MatchObj = gPlaceholderPattern.search(Template, SearchFrom)
            if not MatchObj:
                if MatchEnd <= len(Template):
                    TemplateSection = TemplateString.Section(Template[SectionStart:], PlaceHolderList)
                    TemplateSectionList.append(TemplateSection)
                break

            MatchString = MatchObj.group(1)
            MatchStart = MatchObj.start()
            MatchEnd = MatchObj.end()

            if MatchString == self._REPEAT_START_FLAG:
                if MatchStart > SectionStart:
                    TemplateSection = TemplateString.Section(Template[SectionStart:MatchStart], PlaceHolderList)
                    TemplateSectionList.append(TemplateSection)
                SectionStart = MatchEnd
                PlaceHolderList = []
            elif MatchString == self._REPEAT_END_FLAG:
                TemplateSection = TemplateString.Section(Template[SectionStart:MatchStart], PlaceHolderList)
                TemplateSectionList.append(TemplateSection)
                SectionStart = MatchEnd
                PlaceHolderList = []
            else:
                PlaceHolderList.append((MatchString, MatchStart - SectionStart, MatchEnd - SectionStart))
            SearchFrom = MatchEnd
        return TemplateSectionList

    ## Replace the string template with dictionary of placeholders and append it to previous one
    #
    #   @param      AppendString    The string template to append
    #   @param      Dictionary      The placeholder dictionaries
    #
    def Append(self, AppendString, Dictionary=None):
        if Dictionary:
            SectionList = self._Parse(AppendString)
            self.String += "".join(S.Instantiate(Dictionary) for S in SectionList)
        else:
            self.String += AppendString

    ## Replace the string template with dictionary of placeholders
    #
    #   @param      Dictionary      The placeholder dictionaries
    #
    #   @retval     str             The string replaced with placeholder values
    #
    def Replace(self, Dictionary=None):
        return "".join(S.Instantiate(Dictionary) for S in self._TemplateSectionList)





gInfHeaderString = TemplateString("""\
## @file
# Component description file for ${FileName}Test module.
#
# Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
# This program and the accompanying materials
# are licensed and made available under the terms and conditions of the BSD License
# which accompanies this distribution.  The full text of the license may be found at
# http://opensource.org/licenses/bsd-license.php.
#
# THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
# WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

""")

gInfDefinesString = TemplateString("""\
[Defines]
  INF_VERSION                    = 0x00010005
  BASE_NAME                      = ${FileName}Test
  FILE_GUID                      = 00000000-0000-0000-0000-000000000000
  MODULE_TYPE                    = USER_DEFINED
  VERSION_STRING                 = 1.0

""")

gInfSourcesString = """\
[Sources]
"""

gInfRestsString = TemplateString("""\
[Packages]
  MdePkg/MdePkg.dec
  UnitTestPkg/UnitTestPkg.dec
  UefiHostUnitTestPkg/UefiHostUnitTestPkg.dec
  ${ModulePkgName}/${ModulePkgName}.dec
  ${TestPkgName}/${TestPkgName}.dec

[LibraryClasses]

[Pcd]
  gUefiHostUnitTestPkgTokenSpaceGuid.HostUnitTestMode

""")

gHeaderString = TemplateString("""\
/** @file
  Test file for ${FileName}

  Copyright (c) 2019, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent.

**/

""")

gHIncludePreString = TemplateString("""\
#ifndef _${ModuleName}_TEST_H_
#define _${ModuleName}_TEST_H_

""")

gHIncludePostString = """\

#endif

"""

gCHIncludeString = TemplateString("""\

#include <Uefi.h>
#include <UnitTestTypes.h>
#include <Library/UnitTestLib.h>
#include <Library/UnitTestAssertLib.h>

#include <Library/${HeaderFileName}>

""")

gCIncludeString = TemplateString("""\
#include "${ModuleName}Test.h"

""")

gHIncludeString = """\

#define UNIT_TEST_NAME        L"${ModuleName} Unit Test"
#define UNIT_TEST_VERSION     L"0.1"

typedef struct {
  CHAR16               *Description;
  CHAR16               *ClassName;
  UNIT_TEST_FUNCTION   Func;
  UNIT_TEST_PREREQ     PreReq;
  UNIT_TEST_CLEANUP    CleanUp;
  UNIT_TEST_CONTEXT    Context;
} TEST_DESC;

typedef struct {
  CHAR16                     *Title;
  CHAR16                     *Package;
  UNIT_TEST_SUITE_SETUP      Sup;
  UNIT_TEST_SUITE_TEARDOWN   Tdn;
  UINTN                      *TestNum;
  TEST_DESC                  *TestDesc;
} SUITE_DESC;

"""

gCCaseMainBeginString = """\

SUITE_DESC  mSuiteDesc[] = {
"""

gCCaseMainString = TemplateString("""\
  {L"test of ${CaseName}()", L"${CaseName}.Basic", NULL, NULL, &m${CaseName}TestNum, m${CaseName}Test },
""")

gCCaseMainEndString = """\
};
"""

gCCaseMainRestString = TemplateString("""\

/**
  The main() function for setting up and running the tests.

  @retval EFI_SUCCESS on successful running.
  @retval Other error code on failure.
**/
int main()
{
  UINTN                     SuiteIndex;
  UINTN                     TestIndex;
  EFI_STATUS                Status;
  UNIT_TEST_FRAMEWORK       *Fw = NULL;
  CHAR16                    ShortName[100];

  ShortName[0] = L'\0';

  UnicodeSPrint (&ShortName[0], sizeof(ShortName), L"%a", gEfiCallerBaseName); 
  DEBUG((DEBUG_INFO, "%s v%s\n", UNIT_TEST_NAME, UNIT_TEST_VERSION));

  Status = InitUnitTestFramework (&Fw, UNIT_TEST_NAME, ShortName, UNIT_TEST_VERSION);
  if (EFI_ERROR(Status)) {
    goto EXIT;
  }

  for (SuiteIndex = 0; SuiteIndex < ARRAY_SIZE(mSuiteDesc); SuiteIndex++) {
    UNIT_TEST_SUITE *Suite = NULL;
    Status = CreateUnitTestSuite (&Suite, Fw, mSuiteDesc[SuiteIndex].Title, mSuiteDesc[SuiteIndex].Package, mSuiteDesc[SuiteIndex].Sup, mSuiteDesc[SuiteIndex].Tdn);
    if (EFI_ERROR (Status)) {
      Status = EFI_OUT_OF_RESOURCES;
      goto EXIT;
    }
    for (TestIndex = 0; TestIndex < *mSuiteDesc[SuiteIndex].TestNum; TestIndex++) {
      AddTestCase (Suite, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->Description, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->ClassName, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->Func, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->PreReq, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->CleanUp, (mSuiteDesc[SuiteIndex].TestDesc + TestIndex)->Context);
    }
  }

  Status = RunAllTestSuites (Fw);

EXIT:
  if (Fw != NULL) {
    FreeUnitTestFramework (Fw);
  }

  return Status;
}
""")

gCCaseMainHString = TemplateString("""\
TEST_DESC m${CaseName}Test[];
UINTN m${CaseName}TestNum;

""")

gCCaseString = TemplateString("""\

UNIT_TEST_STATUS
EFIAPI
${CaseName}TestFunc1 (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  return UNIT_TEST_PASSED;
}

UNIT_TEST_STATUS
EFIAPI
${CaseName}TestConf1 (
  IN UNIT_TEST_FRAMEWORK_HANDLE  Framework,
  IN UNIT_TEST_CONTEXT           Context
  )
{
  return UNIT_TEST_PASSED;
}

TEST_DESC m${CaseName}Test[] = {
  { "${CaseName}TestFunc1(): ", L"${CaseName}.Basic.Func", ${CaseName}TestFunc1, NULL, NULL, NULL },
  { "${CaseName}TestConf1(): ", L"${CaseName}.Basic.Conf", ${CaseName}TestConf1, NULL, NULL, NULL }
};

UINTN m${CaseName}TestNum = ARRAY_SIZE(m${CaseName}Test);

""")


class TestCaseTemplate:
    def __init__(self):
        self.FunctionList = []
        self.ModuleLocation = ""
        self.TestPkgPath = ""
        self.ModuleName = ""
        self.HeaderFileName = ""
        self.ModulePkgName = ""
        self.TestPkgName = ""

    def CreateFuncList(self, libHFile):
        f = open(libHFile, 'r')
        lines = f.readlines ()
        foundEFIAPI = False
        for line in lines:
            line.strip()
            if cmp (line[:-1], "EFIAPI") == 0:
                foundEFIAPI = True
                #print('found EFIAPI')
                continue
            if foundEFIAPI :
                word = re.split('[ (]', line)
                self.FunctionList.append(word[0])
                foundEFIAPI = False
        #for func in self.FunctionList:
        #    print("func - \"" + func + "\"")
        f.close()
        word = re.split('[\\\\/]', libHFile)
        self.HeaderFileName = word[-1]

    def CreateModuleLocation(self, ModuleLocation):
        self.ModuleLocation = ModuleLocation
        word = re.split('[\\\\/]', self.ModuleLocation)
        self.ModuleName = word[-1]
        self.ModulePkgName = word[0]

    def CreateTestPkgPath(self, TestPkgPath):
        self.TestPkgPath = TestPkgPath
        word = re.split('[\\\\/]', TestPkgPath)
        self.TestPkgName = word[-1]

    def CreateCaseDir(self):
        caseDir= str(self.TestPkgPath) + "/" + str("TestCase") + "/" + str(self.ModuleLocation) + "/"
        if not os.path.exists(caseDir):
            os.makedirs(caseDir)

    def CreateCaseInf(self):
        caseInf= str(self.TestPkgPath) + "/" + str("TestCase") + "/" + str(self.ModuleLocation) + "/" + str(self.ModuleName) + "Test" + ".inf"
        if os.path.exists(caseInf):
            os.remove(caseInf)
        f = open(caseInf, 'w')
        infText = TemplateString()
        infText.Append(gInfHeaderString.Replace({'FileName':self.ModuleName}))
        infText.Append(gInfDefinesString.Replace({'FileName':self.ModuleName}))
        infText.Append(gInfSourcesString)
        infText.Append("  " + self.ModuleName + "Test.c\n")
        for list in self.FunctionList :
            infText.Append("  " + list + "Test.c\n")
        infText.Append("\n")
        infText.Append(gInfRestsString.Replace({'TestPkgName':self.TestPkgName, 'ModulePkgName':self.ModulePkgName}))
        f.write(str(infText))
        f.close()

    def CreateCaseMainC(self):
        caseMainC = str(self.TestPkgPath) + "/" + str("TestCase") + "/" + str(self.ModuleLocation) + "/" + str(self.ModuleName) + "Test" + ".c"
        if os.path.exists(caseMainC):
            os.remove(caseMainC)
        f = open(caseMainC, 'w')
        cText = TemplateString()
        cText.Append(gHeaderString.Replace({'FileName':self.ModuleName}))
        cText.Append(gCHIncludeString.Replace({'HeaderFileName':self.HeaderFileName}))
        cText.Append(gCIncludeString.Replace({'ModuleName':self.ModuleName}))
        cText.Append(gCCaseMainBeginString)
        for list in self.FunctionList :
            cText.Append(gCCaseMainString.Replace({'CaseName':list}))
        cText.Append(gCCaseMainEndString)
        cText.Append(gCCaseMainRestString.Replace({'ModuleName':self.ModuleName}))
        f.write(str(cText))
        f.close()

    def CreateCaseMainH(self):
        caseMainH = str(self.TestPkgPath) + "/" + str("TestCase") + "/" + str(self.ModuleLocation) + "/" + str(self.ModuleName) + "Test" + ".h"
        if os.path.exists(caseMainH):
            os.remove(caseMainH)
        f = open(caseMainH, 'w')
        hText = TemplateString()
        hText.Append(gHeaderString.Replace({'FileName':self.ModuleName}))
        hText.Append(gHIncludePreString.Replace({'ModuleName':self.ModuleName}))
        hText.Append(gCHIncludeString.Replace({'HeaderFileName':self.HeaderFileName}))
        hText.Append(gHIncludeString)
        for list in self.FunctionList :
            hText.Append(gCCaseMainHString.Replace({'CaseName':list}))
        hText.Append(gHIncludePostString)
        f.write(str(hText))
        f.close()

    def CreateCaseC(self):
        for list in self.FunctionList :
          caseC = str(self.TestPkgPath) + "/" + str("TestCase") + "/" + str(self.ModuleLocation) + "/" + list + "Test" + ".c"
          if os.path.exists(caseC):
              os.remove(caseC)
          f = open(caseC, 'w')
          cText = TemplateString()
          cText.Append(gHeaderString.Replace({'FileName':list}))
          cText.Append(gCHIncludeString.Replace({'HeaderFileName':self.HeaderFileName}))
          cText.Append(gCIncludeString.Replace({'ModuleName':self.ModuleName}))
          cText.Append(gCCaseString.Replace({'CaseName':list}))
          f.write(str(cText))
          f.close()

def usage():
    print("Usage:")
    print("\tGenTestCaseTemplate <HeadFilePath> <ModuleLocation> <TestPkgPath>")
    print("\t\t <HeadFilePath>    The path of the header file")
    print("\t\t <ModuleLocation>  The location of the module")
    print("\t\t <TestPkgPath>     The path of the TestPkg.")
    #print("Example:")
    #print("\tGenTestCaseTemplate.py \home\XXXPkg\Include\Library\YYYLib.h XXXPkg\Library\ZZZLib .\Host\XXXCasePkg")

def main():
    global TestCaseTemplate

    if len(sys.argv) < 4:
        usage()
        return 1

    testCaseTemplate = TestCaseTemplate()

    testCaseTemplate.CreateFuncList (sys.argv[1])
    testCaseTemplate.CreateModuleLocation (sys.argv[2])
    testCaseTemplate.CreateTestPkgPath (sys.argv[3])

    testCaseTemplate.CreateCaseDir()
    testCaseTemplate.CreateCaseInf()
    testCaseTemplate.CreateCaseMainC()
    testCaseTemplate.CreateCaseMainH()
    testCaseTemplate.CreateCaseC()

if __name__ == '__main__':
    sys.exit(main())
