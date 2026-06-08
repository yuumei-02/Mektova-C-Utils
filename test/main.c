#define VTEST_IMPL
#include <vtest.h>

#include <mcu/unicode.h>
#include <mcu/memory.h>

static Arena scratch_arena;

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

TestResult Test_ustr32_len(TestResult previous) {
   unused previous;

   ustr32 msg = U"привет Yuumei";
   usize msg_len = ustr32_len(msg);

   if (msg_len != 13)
      return TR_Fail;
   return TR_Pass;
}

TestResult Test_ustr32_cmp(TestResult previous) {
   unused previous;

   ustr32 msg = U"привет Yuumei";
   if (!ustr32_cmp(msg, msg))                  return TR_Fail;
   if (ustr32_cmp(U"zhyivannye", U"miratte"))  return TR_Fail;
   if (ustr32_cmp(U"mirratte", U"zhyivannye")) return TR_Fail;
   if (!ustr32_cmp(U"", U""))                  return TR_Fail;

   return TR_Pass;
}

TestResult Test_UString32_from(TestResult previous) {
   if (previous != TR_Pass)
      return TR_Skip;

   ustr32 msg_literal = U"привет Yuumei";
   UString32 msg1 = UString32_from(msg_literal);
   UString32 msg2 = UString32_from(msg_literal, .arena = &scratch_arena);

   usize lit_len = ustr32_len(msg_literal);
   if (msg1.length != lit_len || msg2.length != lit_len) return TR_Fail;
   if (msg1.length != msg1.capacity || msg2.length != msg2.capacity) return TR_Fail;

   if (!ustr32_cmp(msg_literal, msg1.chars)) return TR_Fail;
   if (!ustr32_cmp(msg_literal, msg2.chars)) return TR_Fail;

   UString32_delete(&msg1);
   UString32_delete(&msg2, .arena = &scratch_arena);

   return TR_Pass;
}

TestResult Test_UString32_append(TestResult previous) {
   if (previous != TR_Pass)
      return TR_Skip;

   UString32 msg1 = UString32_from(U"привет Yuumei");
   UString32 msg2 = UString32_from(U"привет Yuumei", .arena = &scratch_arena);

   for (usize i = 0; i < 8; ++i) {
      UString32_append(&msg1, U'п');
      UString32_append(&msg2, U'п', .arena = &scratch_arena);
   }

   if (!ustr32_cmp(msg1.chars, U"привет Yuumeiпппппппп")) return TR_Fail;
   if (!ustr32_cmp(msg1.chars, msg2.chars)) return TR_Fail;

   UString32_append_ustr32(&msg1, U"привет");
   UString32_append_ustr32(&msg2, U"привет", .arena = &scratch_arena);

   if (!ustr32_cmp(msg1.chars, U"привет Yuumeiпппппппппривет")) return TR_Fail;
   if (!ustr32_cmp(msg1.chars, msg2.chars)) return TR_Fail;

   UString32_delete(&msg1);
   UString32_delete(&msg2, .arena = &scratch_arena);

   return TR_Pass;
}

TestResult Test_UString32_remove(TestResult previous) {
   if (previous != TR_Pass)
      return TR_Skip;

   UString32 msg1 = UString32_from(U"привет Yuumei");
   UString32 msg2 = UString32_from(U"привет Yuumei", .arena = &scratch_arena);

   u32 popped = UString32_pop(&msg1);
   if (popped != U'i') return TR_Fail;
   popped = UString32_pop(&msg2, .arena = &scratch_arena);
   if (popped != U'i') return TR_Fail;

   popped = UString32_remove(&msg1, 1);
   // @note: This is the cyrillic R, not the ascii one.
   if (popped != U'р') return TR_Fail;
   popped = UString32_remove(&msg2, 1, .arena = &scratch_arena);
   if (popped != U'р') return TR_Fail;
   
   if (!ustr32_cmp(msg1.chars, U"пивет Yuume")) return TR_Fail;
   if (!ustr32_cmp(msg1.chars, msg2.chars)) return TR_Fail;

   UString32_delete(&msg1);
   UString32_delete(&msg2, .arena = &scratch_arena);

   return TR_Pass;
}

i32 main() {
   scratch_arena = Arena_new(MiB, .protection = MP_Read | MP_Write);

   Vtest_start(40);
   run_test_ex(&Test_StringView_chop_left, "SV_chop_left", TR_Unknown);
   run_test_ex(&Test_StringView_chop_right, "SV_chop_right", TR_Unknown);
   run_test_ex(&Test_StringView_chop_left_by_delimiter, "SV_chop_left_by_delimiter", TR_Unknown);
   run_test_ex(&Test_StringView_chop_right_by_delimiter, "SV_chop_right_by_delimiter", TR_Unknown);
   
   TestResult run_from;
   TestResult previous;
   run_from = run_test_ex(&Test_ustr32_len, "ustr32_len", TR_Unknown);
   previous = run_test_ex(&Test_ustr32_cmp, "ustr32_cmp", TR_Unknown);
   if (previous != TR_Pass)
      run_from = previous;

   previous = run_test_ex(&Test_UString32_from, "UString32_from", run_from);
   run_test_ex(&Test_UString32_append, "UString32_append", previous);
   run_test_ex(&Test_UString32_remove, "UString32_remove", previous);
   
   Vtest_end();

   Arena_delete(&scratch_arena);
}

