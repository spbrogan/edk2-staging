/** @file -- SampleUnitTestHost.c

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.


Copyright (C) 2016 Microsoft Corporation. All Rights Reserved.

**/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <Uefi.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

BOOLEAN       mSampleGlobalTestBoolean = FALSE;
VOID          *mSampleGlobalTestPointer = NULL;


///================================================================================================
///================================================================================================
///
/// HELPER FUNCTIONS
///
///================================================================================================
///================================================================================================


//
// Anything you think might be helpful that isn't a test itself.
//

int MakeSureThatPointerIsNull (void **state)
{
  assert_int_equal(mSampleGlobalTestPointer,          NULL);
  mSampleGlobalTestPointer = NULL;
  return 0;
} // ListsShouldHaveTheSameDescriptorSize()


int ClearThePointer (void **state)
{
  mSampleGlobalTestPointer = NULL;
  return 0;
} // ClearThePointer()


///================================================================================================
///================================================================================================
///
/// TEST CASES
///
///================================================================================================
///================================================================================================


void OnePlusOneShouldEqualTwo(void **state)
{
  UINTN     A, B, C;

  A = 1;
  B = 1;
  C = A + B;

  assert_int_equal(C, 2);
} // OnePlusOneShouldEqualTwo()


void GlobalBooleanShouldBeChangeable(void **state)
{
  mSampleGlobalTestBoolean = TRUE;
  assert_true(mSampleGlobalTestBoolean);
  
  mSampleGlobalTestBoolean = FALSE;
  assert_false(mSampleGlobalTestBoolean);
} // GlobalBooleanShouldBeChangeable()


void GlobalPointerShouldBeChangeable(void **state)
{
  mSampleGlobalTestPointer = (VOID*)-1;
  assert_int_equal(mSampleGlobalTestPointer, ((VOID*)-1));
} // GlobalPointerShouldBeChangeable()


///================================================================================================
///================================================================================================
///
/// TEST ENGINE
///
///================================================================================================
///================================================================================================


/** 
  SampleUnitTestApp
  
  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point executed successfully.
  @retval other           Some error occured when executing this entry point.

**/
int main ()
{
  if (PcdGet8 (HostUnitTestMode) == 1) {
    cmocka_set_message_output (CM_OUTPUT_XML);
  }

  const struct CMUnitTest SimpleMatchTests[] = {
    cmocka_unit_test(OnePlusOneShouldEqualTwo),
  };

  cmocka_run_group_tests(SimpleMatchTests, NULL, NULL);

  const struct CMUnitTest GlobalVariableTests[] = {
    cmocka_unit_test(GlobalBooleanShouldBeChangeable),
    cmocka_unit_test_setup_teardown(GlobalPointerShouldBeChangeable, MakeSureThatPointerIsNull, ClearThePointer),
  };

  cmocka_run_group_tests(GlobalVariableTests, NULL, NULL);

  return 0;
}