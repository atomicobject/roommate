#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(LEDLogic)
{
  RUN_TEST_CASE(LEDLogic, all_leds_should_be_green_when_no_meetings_scheduled);
  // RUN_TEST_CASE(ProductionCode, FindFunction_WhichIsBroken_ShouldReturnZeroIfItemIsNotInList_WhichWorksEvenInOurBrokenCode);
  // RUN_TEST_CASE(ProductionCode, FindFunction_WhichIsBroken_ShouldReturnTheIndexForItemsInList_WhichWillFailBecauseOurFunctionUnderTestIsBroken);
  // RUN_TEST_CASE(ProductionCode, FunctionWhichReturnsLocalVariable_ShouldReturnTheCurrentCounterValue);
  // RUN_TEST_CASE(ProductionCode, FunctionWhichReturnsLocalVariable_ShouldReturnTheCurrentCounterValueAgain);
  // RUN_TEST_CASE(ProductionCode, FunctionWhichReturnsLocalVariable_ShouldReturnCurrentCounter_ButFailsBecauseThisTestIsActuallyFlawed);
}