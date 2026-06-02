#define VTEST_IMPL
#include <vtest.h>

bool test_sv_equals(StringView left, StringView right) {
   if (left.length != right.length) return false;

   for (usize i = 0; i < left.length; ++i) {
      if (left.chars[i] != right.chars[i])
         return false;
   }

   return true;
}

bool test_sv_streq(StringView self, cstr str) {
   return test_sv_equals(self, (StringView) {
      .chars = str,
      .length = strlen(str)
   });
}

TestResult Test_StringView_chop_left(TestResult previous) {
   unused previous;
   cstr msg = "zhyivannye miratte";

   // part 1
   StringView sv = StringView_from(msg, strlen(msg));
   StringView chopped = SV_chop_left(&sv, strlen("zhyivannye"));

   if (!test_sv_streq(sv, " miratte")) return TR_Fail;
   if (!test_sv_streq(chopped, "zhyivannye") != 0) return TR_Fail;

   // part 2
   sv = StringView_from(msg, strlen(msg));
   chopped = SV_chop_left(&sv, 69420);
   if (!test_sv_streq(chopped, msg)) return TR_Fail;
   if (!test_sv_streq(sv, "")) return TR_Fail;

   return TR_Pass;
}

TestResult Test_StringView_chop_right(TestResult previous) {
   unused previous;
   cstr msg = "zhyivannye miratte";

   // part 1
   StringView sv = StringView_from(msg, strlen(msg));
   StringView chopped = SV_chop_right(&sv, strlen("miratte"));

   if (!test_sv_streq(sv, "zhyivannye ")) return TR_Fail;
   if (!test_sv_streq(chopped, "miratte") != 0) return TR_Fail;

   // part 2
   sv = StringView_from(msg, strlen(msg));
   chopped = SV_chop_right(&sv, 69420);
   if (!test_sv_streq(chopped, msg)) return TR_Fail;
   if (!test_sv_streq(sv, "")) return TR_Fail;

   return TR_Pass;
}

TestResult Test_StringView_chop_left_by_delimiter(TestResult previous) {
   unused previous;
   cstr msg1 = "zhyivannye miratte";
   cstr msg2 = "zhyivannye,";
   cstr msg3 = ",zhyivannye";

   // part 1
   StringView sv = StringView_from(msg1, strlen(msg1));
   StringView chopped = SV_chop_left_by_delimiter(&sv, ' ');

   if (!test_sv_streq(sv, "miratte")) return TR_Fail;
   if (!test_sv_streq(chopped, "zhyivannye")) return TR_Fail;

   // part 2
   sv = StringView_from(msg2, strlen(msg2));
   chopped = SV_chop_left_by_delimiter(&sv, ',');

   if (!test_sv_streq(sv, "")) return TR_Fail;
   if (!test_sv_streq(chopped, "zhyivannye")) return TR_Fail;

   // part 3
   sv = StringView_from(msg3, strlen(msg3));
   chopped = SV_chop_left_by_delimiter(&sv, ',');

   if (!test_sv_streq(sv, "zhyivannye")) return TR_Fail;
   if (!test_sv_streq(chopped, "")) return TR_Fail;

   return TR_Pass;
}

TestResult Test_StringView_chop_right_by_delimiter(TestResult previous) {
   unused previous;
   cstr msg1 = "zhyivannye miratte";
   cstr msg2 = "zhyivannye,";
   cstr msg3 = ",zhyivannye";

   // part 1
   StringView sv = StringView_from(msg1, strlen(msg1));
   StringView chopped = SV_chop_right_by_delimiter(&sv, ' ');

   if (!test_sv_streq(sv, "zhyivannye")) return TR_Fail;
   if (!test_sv_streq(chopped, "miratte")) return TR_Fail;

   // part 2
   sv = StringView_from(msg2, strlen(msg2));
   chopped = SV_chop_right_by_delimiter(&sv, ',');

   if (!test_sv_streq(sv, "zhyivannye")) return TR_Fail;
   if (!test_sv_streq(chopped, "")) return TR_Fail;

   // part 3
   sv = StringView_from(msg3, strlen(msg3));
   chopped = SV_chop_right_by_delimiter(&sv, ',');

   if (!test_sv_streq(sv, "")) return TR_Fail;
   if (!test_sv_streq(chopped, "zhyivannye")) return TR_Fail;

   return TR_Pass;
}

i32 main() {
   Vtest_start(40);
   run_test_ex(&Test_StringView_chop_left, "SV_chop_left", TR_Unknown);
   run_test_ex(&Test_StringView_chop_right, "SV_chop_right", TR_Unknown);
   run_test_ex(&Test_StringView_chop_left_by_delimiter, "SV_chop_left_by_delimiter", TR_Unknown);
   run_test_ex(&Test_StringView_chop_right_by_delimiter, "SV_chop_right_by_delimiter", TR_Unknown);
   Vtest_end();
}

