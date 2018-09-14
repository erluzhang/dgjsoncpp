// Copyright 2007-2010 Baptiste Lepilleur and The JsonCpp Authors
// Distributed under MIT license, or public domain if desired and
// recognized in your jurisdiction.
// See file LICENSE for detail or copy at http://jsoncpp.sourceforge.net/LICENSE

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(disable : 4996)
#endif

#include "jsontest.h"
#include <cmath>
#include <cstring>
#include <iomanip>
#include <json/config.h>
#include <json/json.h>
#include <limits>
#include <sstream>
#include <string>

// Make numeric limits more convenient to talk about.
// Assumes int type in 32 bits.
#define kint32max dgJson::Value::maxInt
#define kint32min dgJson::Value::minInt
#define kuint32max dgJson::Value::maxUInt
#define kint64max dgJson::Value::maxInt64
#define kint64min dgJson::Value::minInt64
#define kuint64max dgJson::Value::maxUInt64

// static const double kdint64max = double(kint64max);
// static const float kfint64max = float(kint64max);
static const float kfint32max = float(kint32max);
static const float kfuint32max = float(kuint32max);

// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////
// Json Library test cases
// //////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////

#if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
static inline double uint64ToDouble(dgJson::UInt64 value) {
  return static_cast<double>(value);
}
#else  // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)
static inline double uint64ToDouble(dgJson::UInt64 value) {
  return static_cast<double>(dgJson::Int64(value / 2)) * 2.0 +
         static_cast<double>(dgJson::Int64(value & 1));
}
#endif // if !defined(JSON_USE_INT64_DOUBLE_CONVERSION)

struct ValueTest : JsonTest::TestCase {
  dgJson::Value null_;
  dgJson::Value emptyArray_;
  dgJson::Value emptyObject_;
  dgJson::Value integer_;
  dgJson::Value unsignedInteger_;
  dgJson::Value smallUnsignedInteger_;
  dgJson::Value real_;
  dgJson::Value float_;
  dgJson::Value array1_;
  dgJson::Value object1_;
  dgJson::Value emptyString_;
  dgJson::Value string1_;
  dgJson::Value string_;
  dgJson::Value true_;
  dgJson::Value false_;

  ValueTest()
      : emptyArray_(dgJson::arrayValue), emptyObject_(dgJson::objectValue),
        integer_(123456789), unsignedInteger_(34567890u),
        smallUnsignedInteger_(dgJson::Value::UInt(dgJson::Value::maxInt)),
        real_(1234.56789), float_(0.00390625f), emptyString_(""), string1_("a"),
        string_("sometext with space"), true_(true), false_(false) {
    array1_.append(1234);
    object1_["id"] = 1234;
  }

  struct IsCheck {
    /// Initialize all checks to \c false by default.
    IsCheck();

    bool isObject_;
    bool isArray_;
    bool isBool_;
    bool isString_;
    bool isNull_;

    bool isInt_;
    bool isInt64_;
    bool isUInt_;
    bool isUInt64_;
    bool isIntegral_;
    bool isDouble_;
    bool isNumeric_;
  };

  void checkConstMemberCount(const dgJson::Value& value,
                             unsigned int expectedCount);

  void checkMemberCount(dgJson::Value& value, unsigned int expectedCount);

  void checkIs(const dgJson::Value& value, const IsCheck& check);

  void checkIsLess(const dgJson::Value& x, const dgJson::Value& y);

  void checkIsEqual(const dgJson::Value& x, const dgJson::Value& y);

  /// Normalize the representation of floating-point number by stripped leading
  /// 0 in exponent.
  static JSONCPP_STRING normalizeFloatingPointStr(const JSONCPP_STRING& s);
};

JSONCPP_STRING ValueTest::normalizeFloatingPointStr(const JSONCPP_STRING& s) {
  JSONCPP_STRING::size_type index = s.find_last_of("eE");
  if (index != JSONCPP_STRING::npos) {
    JSONCPP_STRING::size_type hasSign =
        (s[index + 1] == '+' || s[index + 1] == '-') ? 1 : 0;
    JSONCPP_STRING::size_type exponentStartIndex = index + 1 + hasSign;
    JSONCPP_STRING normalized = s.substr(0, exponentStartIndex);
    JSONCPP_STRING::size_type indexDigit =
        s.find_first_not_of('0', exponentStartIndex);
    JSONCPP_STRING exponent = "0";
    if (indexDigit != JSONCPP_STRING::npos) // There is an exponent different
                                            // from 0
    {
      exponent = s.substr(indexDigit);
    }
    return normalized + exponent;
  }
  return s;
}

JSONTEST_FIXTURE(ValueTest, checkNormalizeFloatingPointStr) {
  JSONTEST_ASSERT_STRING_EQUAL("0.0", normalizeFloatingPointStr("0.0"));
  JSONTEST_ASSERT_STRING_EQUAL("0e0", normalizeFloatingPointStr("0e0"));
  JSONTEST_ASSERT_STRING_EQUAL("1234.0", normalizeFloatingPointStr("1234.0"));
  JSONTEST_ASSERT_STRING_EQUAL("1234.0e0",
                               normalizeFloatingPointStr("1234.0e0"));
  JSONTEST_ASSERT_STRING_EQUAL("1234.0e+0",
                               normalizeFloatingPointStr("1234.0e+0"));
  JSONTEST_ASSERT_STRING_EQUAL("1234e-1", normalizeFloatingPointStr("1234e-1"));
  JSONTEST_ASSERT_STRING_EQUAL("1234e10", normalizeFloatingPointStr("1234e10"));
  JSONTEST_ASSERT_STRING_EQUAL("1234e10",
                               normalizeFloatingPointStr("1234e010"));
  JSONTEST_ASSERT_STRING_EQUAL("1234e+10",
                               normalizeFloatingPointStr("1234e+010"));
  JSONTEST_ASSERT_STRING_EQUAL("1234e-10",
                               normalizeFloatingPointStr("1234e-010"));
  JSONTEST_ASSERT_STRING_EQUAL("1234e+100",
                               normalizeFloatingPointStr("1234e+100"));
  JSONTEST_ASSERT_STRING_EQUAL("1234e-100",
                               normalizeFloatingPointStr("1234e-100"));
  JSONTEST_ASSERT_STRING_EQUAL("1234e+1",
                               normalizeFloatingPointStr("1234e+001"));
}

JSONTEST_FIXTURE(ValueTest, memberCount) {
  JSONTEST_ASSERT_PRED(checkMemberCount(emptyArray_, 0));
  JSONTEST_ASSERT_PRED(checkMemberCount(emptyObject_, 0));
  JSONTEST_ASSERT_PRED(checkMemberCount(array1_, 1));
  JSONTEST_ASSERT_PRED(checkMemberCount(object1_, 1));
  JSONTEST_ASSERT_PRED(checkMemberCount(null_, 0));
  JSONTEST_ASSERT_PRED(checkMemberCount(integer_, 0));
  JSONTEST_ASSERT_PRED(checkMemberCount(unsignedInteger_, 0));
  JSONTEST_ASSERT_PRED(checkMemberCount(smallUnsignedInteger_, 0));
  JSONTEST_ASSERT_PRED(checkMemberCount(real_, 0));
  JSONTEST_ASSERT_PRED(checkMemberCount(emptyString_, 0));
  JSONTEST_ASSERT_PRED(checkMemberCount(string_, 0));
  JSONTEST_ASSERT_PRED(checkMemberCount(true_, 0));
}

JSONTEST_FIXTURE(ValueTest, objects) {
  // Types
  IsCheck checks;
  checks.isObject_ = true;
  JSONTEST_ASSERT_PRED(checkIs(emptyObject_, checks));
  JSONTEST_ASSERT_PRED(checkIs(object1_, checks));

  JSONTEST_ASSERT_EQUAL(dgJson::objectValue, emptyObject_.type());

  // Empty object okay
  JSONTEST_ASSERT(emptyObject_.isConvertibleTo(dgJson::nullValue));

  // Non-empty object not okay
  JSONTEST_ASSERT(!object1_.isConvertibleTo(dgJson::nullValue));

  // Always okay
  JSONTEST_ASSERT(emptyObject_.isConvertibleTo(dgJson::objectValue));

  // Never okay
  JSONTEST_ASSERT(!emptyObject_.isConvertibleTo(dgJson::arrayValue));
  JSONTEST_ASSERT(!emptyObject_.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!emptyObject_.isConvertibleTo(dgJson::uintValue));
  JSONTEST_ASSERT(!emptyObject_.isConvertibleTo(dgJson::realValue));
  JSONTEST_ASSERT(!emptyObject_.isConvertibleTo(dgJson::booleanValue));
  JSONTEST_ASSERT(!emptyObject_.isConvertibleTo(dgJson::stringValue));

  // Access through const reference
  const dgJson::Value& constObject = object1_;

  JSONTEST_ASSERT_EQUAL(dgJson::Value(1234), constObject["id"]);
  JSONTEST_ASSERT_EQUAL(dgJson::Value(), constObject["unknown id"]);

  // Access through non-const reference
  JSONTEST_ASSERT_EQUAL(dgJson::Value(1234), object1_["id"]);
  JSONTEST_ASSERT_EQUAL(dgJson::Value(), object1_["unknown id"]);

  object1_["some other id"] = "foo";
  JSONTEST_ASSERT_EQUAL(dgJson::Value("foo"), object1_["some other id"]);
  JSONTEST_ASSERT_EQUAL(dgJson::Value("foo"), object1_["some other id"]);

  // Remove.
  dgJson::Value got;
  bool did;
  did = object1_.removeMember("some other id", &got);
  JSONTEST_ASSERT_EQUAL(dgJson::Value("foo"), got);
  JSONTEST_ASSERT_EQUAL(true, did);
  got = dgJson::Value("bar");
  did = object1_.removeMember("some other id", &got);
  JSONTEST_ASSERT_EQUAL(dgJson::Value("bar"), got);
  JSONTEST_ASSERT_EQUAL(false, did);
}

JSONTEST_FIXTURE(ValueTest, arrays) {
  const unsigned int index0 = 0;

  // Types
  IsCheck checks;
  checks.isArray_ = true;
  JSONTEST_ASSERT_PRED(checkIs(emptyArray_, checks));
  JSONTEST_ASSERT_PRED(checkIs(array1_, checks));

  JSONTEST_ASSERT_EQUAL(dgJson::arrayValue, array1_.type());

  // Empty array okay
  JSONTEST_ASSERT(emptyArray_.isConvertibleTo(dgJson::nullValue));

  // Non-empty array not okay
  JSONTEST_ASSERT(!array1_.isConvertibleTo(dgJson::nullValue));

  // Always okay
  JSONTEST_ASSERT(emptyArray_.isConvertibleTo(dgJson::arrayValue));

  // Never okay
  JSONTEST_ASSERT(!emptyArray_.isConvertibleTo(dgJson::objectValue));
  JSONTEST_ASSERT(!emptyArray_.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!emptyArray_.isConvertibleTo(dgJson::uintValue));
  JSONTEST_ASSERT(!emptyArray_.isConvertibleTo(dgJson::realValue));
  JSONTEST_ASSERT(!emptyArray_.isConvertibleTo(dgJson::booleanValue));
  JSONTEST_ASSERT(!emptyArray_.isConvertibleTo(dgJson::stringValue));

  // Access through const reference
  const dgJson::Value& constArray = array1_;
  JSONTEST_ASSERT_EQUAL(dgJson::Value(1234), constArray[index0]);
  JSONTEST_ASSERT_EQUAL(dgJson::Value(1234), constArray[0]);

  // Access through non-const reference
  JSONTEST_ASSERT_EQUAL(dgJson::Value(1234), array1_[index0]);
  JSONTEST_ASSERT_EQUAL(dgJson::Value(1234), array1_[0]);

  array1_[2] = dgJson::Value(17);
  JSONTEST_ASSERT_EQUAL(dgJson::Value(), array1_[1]);
  JSONTEST_ASSERT_EQUAL(dgJson::Value(17), array1_[2]);
  dgJson::Value got;
  JSONTEST_ASSERT_EQUAL(true, array1_.removeIndex(2, &got));
  JSONTEST_ASSERT_EQUAL(dgJson::Value(17), got);
  JSONTEST_ASSERT_EQUAL(false, array1_.removeIndex(2, &got)); // gone now
}
JSONTEST_FIXTURE(ValueTest, arrayIssue252) {
  int count = 5;
  dgJson::Value root;
  dgJson::Value item;
  root["array"] = dgJson::Value::nullRef;
  for (int i = 0; i < count; i++) {
    item["a"] = i;
    item["b"] = i;
    root["array"][i] = item;
  }
  // JSONTEST_ASSERT_EQUAL(5, root["array"].size());
}

JSONTEST_FIXTURE(ValueTest, null) {
  JSONTEST_ASSERT_EQUAL(dgJson::nullValue, null_.type());

  IsCheck checks;
  checks.isNull_ = true;
  JSONTEST_ASSERT_PRED(checkIs(null_, checks));

  JSONTEST_ASSERT(null_.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(null_.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(null_.isConvertibleTo(dgJson::uintValue));
  JSONTEST_ASSERT(null_.isConvertibleTo(dgJson::realValue));
  JSONTEST_ASSERT(null_.isConvertibleTo(dgJson::booleanValue));
  JSONTEST_ASSERT(null_.isConvertibleTo(dgJson::stringValue));
  JSONTEST_ASSERT(null_.isConvertibleTo(dgJson::arrayValue));
  JSONTEST_ASSERT(null_.isConvertibleTo(dgJson::objectValue));

  JSONTEST_ASSERT_EQUAL(dgJson::Int(0), null_.asInt());
  JSONTEST_ASSERT_EQUAL(dgJson::LargestInt(0), null_.asLargestInt());
  JSONTEST_ASSERT_EQUAL(dgJson::UInt(0), null_.asUInt());
  JSONTEST_ASSERT_EQUAL(dgJson::LargestUInt(0), null_.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(0.0, null_.asDouble());
  JSONTEST_ASSERT_EQUAL(0.0, null_.asFloat());
  JSONTEST_ASSERT_STRING_EQUAL("", null_.asString());

  JSONTEST_ASSERT_EQUAL(dgJson::Value::null, null_);

  // Test using a Value in a boolean context (false iff null)
  JSONTEST_ASSERT_EQUAL(null_, false);
  JSONTEST_ASSERT_EQUAL(object1_, true);
  JSONTEST_ASSERT_EQUAL(!null_, true);
  JSONTEST_ASSERT_EQUAL(!object1_, false);
}

JSONTEST_FIXTURE(ValueTest, strings) {
  JSONTEST_ASSERT_EQUAL(dgJson::stringValue, string1_.type());

  IsCheck checks;
  checks.isString_ = true;
  JSONTEST_ASSERT_PRED(checkIs(emptyString_, checks));
  JSONTEST_ASSERT_PRED(checkIs(string_, checks));
  JSONTEST_ASSERT_PRED(checkIs(string1_, checks));

  // Empty string okay
  JSONTEST_ASSERT(emptyString_.isConvertibleTo(dgJson::nullValue));

  // Non-empty string not okay
  JSONTEST_ASSERT(!string1_.isConvertibleTo(dgJson::nullValue));

  // Always okay
  JSONTEST_ASSERT(string1_.isConvertibleTo(dgJson::stringValue));

  // Never okay
  JSONTEST_ASSERT(!string1_.isConvertibleTo(dgJson::objectValue));
  JSONTEST_ASSERT(!string1_.isConvertibleTo(dgJson::arrayValue));
  JSONTEST_ASSERT(!string1_.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!string1_.isConvertibleTo(dgJson::uintValue));
  JSONTEST_ASSERT(!string1_.isConvertibleTo(dgJson::realValue));

  JSONTEST_ASSERT_STRING_EQUAL("a", string1_.asString());
  JSONTEST_ASSERT_STRING_EQUAL("a", string1_.asCString());
}

JSONTEST_FIXTURE(ValueTest, bools) {
  JSONTEST_ASSERT_EQUAL(dgJson::booleanValue, false_.type());

  IsCheck checks;
  checks.isBool_ = true;
  JSONTEST_ASSERT_PRED(checkIs(false_, checks));
  JSONTEST_ASSERT_PRED(checkIs(true_, checks));

  // False okay
  JSONTEST_ASSERT(false_.isConvertibleTo(dgJson::nullValue));

  // True not okay
  JSONTEST_ASSERT(!true_.isConvertibleTo(dgJson::nullValue));

  // Always okay
  JSONTEST_ASSERT(true_.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(true_.isConvertibleTo(dgJson::uintValue));
  JSONTEST_ASSERT(true_.isConvertibleTo(dgJson::realValue));
  JSONTEST_ASSERT(true_.isConvertibleTo(dgJson::booleanValue));
  JSONTEST_ASSERT(true_.isConvertibleTo(dgJson::stringValue));

  // Never okay
  JSONTEST_ASSERT(!true_.isConvertibleTo(dgJson::arrayValue));
  JSONTEST_ASSERT(!true_.isConvertibleTo(dgJson::objectValue));

  JSONTEST_ASSERT_EQUAL(true, true_.asBool());
  JSONTEST_ASSERT_EQUAL(1, true_.asInt());
  JSONTEST_ASSERT_EQUAL(1, true_.asLargestInt());
  JSONTEST_ASSERT_EQUAL(1, true_.asUInt());
  JSONTEST_ASSERT_EQUAL(1, true_.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(1.0, true_.asDouble());
  JSONTEST_ASSERT_EQUAL(1.0, true_.asFloat());

  JSONTEST_ASSERT_EQUAL(false, false_.asBool());
  JSONTEST_ASSERT_EQUAL(0, false_.asInt());
  JSONTEST_ASSERT_EQUAL(0, false_.asLargestInt());
  JSONTEST_ASSERT_EQUAL(0, false_.asUInt());
  JSONTEST_ASSERT_EQUAL(0, false_.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(0.0, false_.asDouble());
  JSONTEST_ASSERT_EQUAL(0.0, false_.asFloat());
}

JSONTEST_FIXTURE(ValueTest, integers) {
  IsCheck checks;
  dgJson::Value val;

  // Conversions that don't depend on the value.
  JSONTEST_ASSERT(dgJson::Value(17).isConvertibleTo(dgJson::realValue));
  JSONTEST_ASSERT(dgJson::Value(17).isConvertibleTo(dgJson::stringValue));
  JSONTEST_ASSERT(dgJson::Value(17).isConvertibleTo(dgJson::booleanValue));
  JSONTEST_ASSERT(!dgJson::Value(17).isConvertibleTo(dgJson::arrayValue));
  JSONTEST_ASSERT(!dgJson::Value(17).isConvertibleTo(dgJson::objectValue));

  JSONTEST_ASSERT(dgJson::Value(17U).isConvertibleTo(dgJson::realValue));
  JSONTEST_ASSERT(dgJson::Value(17U).isConvertibleTo(dgJson::stringValue));
  JSONTEST_ASSERT(dgJson::Value(17U).isConvertibleTo(dgJson::booleanValue));
  JSONTEST_ASSERT(!dgJson::Value(17U).isConvertibleTo(dgJson::arrayValue));
  JSONTEST_ASSERT(!dgJson::Value(17U).isConvertibleTo(dgJson::objectValue));

  JSONTEST_ASSERT(dgJson::Value(17.0).isConvertibleTo(dgJson::realValue));
  JSONTEST_ASSERT(dgJson::Value(17.0).isConvertibleTo(dgJson::stringValue));
  JSONTEST_ASSERT(dgJson::Value(17.0).isConvertibleTo(dgJson::booleanValue));
  JSONTEST_ASSERT(!dgJson::Value(17.0).isConvertibleTo(dgJson::arrayValue));
  JSONTEST_ASSERT(!dgJson::Value(17.0).isConvertibleTo(dgJson::objectValue));

  // Default int
  val = dgJson::Value(dgJson::intValue);

  JSONTEST_ASSERT_EQUAL(dgJson::intValue, val.type());

  checks = IsCheck();
  checks.isInt_ = true;
  checks.isInt64_ = true;
  checks.isUInt_ = true;
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(0, val.asInt());
  JSONTEST_ASSERT_EQUAL(0, val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(0, val.asUInt());
  JSONTEST_ASSERT_EQUAL(0, val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(0.0, val.asDouble());
  JSONTEST_ASSERT_EQUAL(0.0, val.asFloat());
  JSONTEST_ASSERT_EQUAL(false, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("0", val.asString());

  // Default uint
  val = dgJson::Value(dgJson::uintValue);

  JSONTEST_ASSERT_EQUAL(dgJson::uintValue, val.type());

  checks = IsCheck();
  checks.isInt_ = true;
  checks.isInt64_ = true;
  checks.isUInt_ = true;
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(0, val.asInt());
  JSONTEST_ASSERT_EQUAL(0, val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(0, val.asUInt());
  JSONTEST_ASSERT_EQUAL(0, val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(0.0, val.asDouble());
  JSONTEST_ASSERT_EQUAL(0.0, val.asFloat());
  JSONTEST_ASSERT_EQUAL(false, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("0", val.asString());

  // Default real
  val = dgJson::Value(dgJson::realValue);

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::uintValue));

  checks = IsCheck();
  checks.isInt_ = true;
  checks.isInt64_ = true;
  checks.isUInt_ = true;
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT_EQUAL(0, val.asInt());
  JSONTEST_ASSERT_EQUAL(0, val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(0, val.asUInt());
  JSONTEST_ASSERT_EQUAL(0, val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(0.0, val.asDouble());
  JSONTEST_ASSERT_EQUAL(0.0, val.asFloat());
  JSONTEST_ASSERT_EQUAL(false, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("0.0", val.asString());

  // Zero (signed constructor arg)
  val = dgJson::Value(0);

  JSONTEST_ASSERT_EQUAL(dgJson::intValue, val.type());

  checks = IsCheck();
  checks.isInt_ = true;
  checks.isInt64_ = true;
  checks.isUInt_ = true;
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(0, val.asInt());
  JSONTEST_ASSERT_EQUAL(0, val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(0, val.asUInt());
  JSONTEST_ASSERT_EQUAL(0, val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(0.0, val.asDouble());
  JSONTEST_ASSERT_EQUAL(0.0, val.asFloat());
  JSONTEST_ASSERT_EQUAL(false, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("0", val.asString());

  // Zero (unsigned constructor arg)
  val = dgJson::Value(0u);

  JSONTEST_ASSERT_EQUAL(dgJson::uintValue, val.type());

  checks = IsCheck();
  checks.isInt_ = true;
  checks.isInt64_ = true;
  checks.isUInt_ = true;
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(0, val.asInt());
  JSONTEST_ASSERT_EQUAL(0, val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(0, val.asUInt());
  JSONTEST_ASSERT_EQUAL(0, val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(0.0, val.asDouble());
  JSONTEST_ASSERT_EQUAL(0.0, val.asFloat());
  JSONTEST_ASSERT_EQUAL(false, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("0", val.asString());

  // Zero (floating-point constructor arg)
  val = dgJson::Value(0.0);

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  checks = IsCheck();
  checks.isInt_ = true;
  checks.isInt64_ = true;
  checks.isUInt_ = true;
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(0, val.asInt());
  JSONTEST_ASSERT_EQUAL(0, val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(0, val.asUInt());
  JSONTEST_ASSERT_EQUAL(0, val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(0.0, val.asDouble());
  JSONTEST_ASSERT_EQUAL(0.0, val.asFloat());
  JSONTEST_ASSERT_EQUAL(false, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("0.0", val.asString());

  // 2^20 (signed constructor arg)
  val = dgJson::Value(1 << 20);

  JSONTEST_ASSERT_EQUAL(dgJson::intValue, val.type());
  checks = IsCheck();
  checks.isInt_ = true;
  checks.isInt64_ = true;
  checks.isUInt_ = true;
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL((1 << 20), val.asInt());
  JSONTEST_ASSERT_EQUAL((1 << 20), val.asLargestInt());
  JSONTEST_ASSERT_EQUAL((1 << 20), val.asUInt());
  JSONTEST_ASSERT_EQUAL((1 << 20), val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL((1 << 20), val.asDouble());
  JSONTEST_ASSERT_EQUAL((1 << 20), val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("1048576", val.asString());

  // 2^20 (unsigned constructor arg)
  val = dgJson::Value(dgJson::UInt(1 << 20));

  JSONTEST_ASSERT_EQUAL(dgJson::uintValue, val.type());

  checks = IsCheck();
  checks.isInt_ = true;
  checks.isInt64_ = true;
  checks.isUInt_ = true;
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL((1 << 20), val.asInt());
  JSONTEST_ASSERT_EQUAL((1 << 20), val.asLargestInt());
  JSONTEST_ASSERT_EQUAL((1 << 20), val.asUInt());
  JSONTEST_ASSERT_EQUAL((1 << 20), val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL((1 << 20), val.asDouble());
  JSONTEST_ASSERT_EQUAL((1 << 20), val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("1048576", val.asString());

  // 2^20 (floating-point constructor arg)
  val = dgJson::Value((1 << 20) / 1.0);

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  checks = IsCheck();
  checks.isInt_ = true;
  checks.isInt64_ = true;
  checks.isUInt_ = true;
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL((1 << 20), val.asInt());
  JSONTEST_ASSERT_EQUAL((1 << 20), val.asLargestInt());
  JSONTEST_ASSERT_EQUAL((1 << 20), val.asUInt());
  JSONTEST_ASSERT_EQUAL((1 << 20), val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL((1 << 20), val.asDouble());
  JSONTEST_ASSERT_EQUAL((1 << 20), val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL(
      "1048576.0",
      normalizeFloatingPointStr(JsonTest::ToJsonString(val.asString())));

  // -2^20
  val = dgJson::Value(-(1 << 20));

  JSONTEST_ASSERT_EQUAL(dgJson::intValue, val.type());

  checks = IsCheck();
  checks.isInt_ = true;
  checks.isInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(-(1 << 20), val.asInt());
  JSONTEST_ASSERT_EQUAL(-(1 << 20), val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(-(1 << 20), val.asDouble());
  JSONTEST_ASSERT_EQUAL(-(1 << 20), val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("-1048576", val.asString());

  // int32 max
  val = dgJson::Value(kint32max);

  JSONTEST_ASSERT_EQUAL(dgJson::intValue, val.type());

  checks = IsCheck();
  checks.isInt_ = true;
  checks.isInt64_ = true;
  checks.isUInt_ = true;
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(kint32max, val.asInt());
  JSONTEST_ASSERT_EQUAL(kint32max, val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(kint32max, val.asUInt());
  JSONTEST_ASSERT_EQUAL(kint32max, val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(kint32max, val.asDouble());
  JSONTEST_ASSERT_EQUAL(kfint32max, val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("2147483647", val.asString());

  // int32 min
  val = dgJson::Value(kint32min);

  JSONTEST_ASSERT_EQUAL(dgJson::intValue, val.type());

  checks = IsCheck();
  checks.isInt_ = true;
  checks.isInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(kint32min, val.asInt());
  JSONTEST_ASSERT_EQUAL(kint32min, val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(kint32min, val.asDouble());
  JSONTEST_ASSERT_EQUAL(kint32min, val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("-2147483648", val.asString());

  // uint32 max
  val = dgJson::Value(kuint32max);

  JSONTEST_ASSERT_EQUAL(dgJson::uintValue, val.type());

  checks = IsCheck();
  checks.isInt64_ = true;
  checks.isUInt_ = true;
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::uintValue));

#ifndef JSON_NO_INT64
  JSONTEST_ASSERT_EQUAL(kuint32max, val.asLargestInt());
#endif
  JSONTEST_ASSERT_EQUAL(kuint32max, val.asUInt());
  JSONTEST_ASSERT_EQUAL(kuint32max, val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(kuint32max, val.asDouble());
  JSONTEST_ASSERT_EQUAL(kfuint32max, val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("4294967295", val.asString());

#ifdef JSON_NO_INT64
  // int64 max
  val = dgJson::Value(double(kint64max));

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  checks = IsCheck();
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(double(kint64max), val.asDouble());
  JSONTEST_ASSERT_EQUAL(float(kint64max), val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("9.22337e+18", val.asString());

  // int64 min
  val = dgJson::Value(double(kint64min));

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  checks = IsCheck();
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(double(kint64min), val.asDouble());
  JSONTEST_ASSERT_EQUAL(float(kint64min), val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("-9.22337e+18", val.asString());

  // uint64 max
  val = dgJson::Value(double(kuint64max));

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  checks = IsCheck();
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(double(kuint64max), val.asDouble());
  JSONTEST_ASSERT_EQUAL(float(kuint64max), val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("1.84467e+19", val.asString());
#else // ifdef JSON_NO_INT64
  // 2^40 (signed constructor arg)
  val = dgJson::Value(dgJson::Int64(1) << 40);

  JSONTEST_ASSERT_EQUAL(dgJson::intValue, val.type());

  checks = IsCheck();
  checks.isInt64_ = true;
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asInt64());
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asLargestInt());
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asUInt64());
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asDouble());
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("1099511627776", val.asString());

  // 2^40 (unsigned constructor arg)
  val = dgJson::Value(dgJson::UInt64(1) << 40);

  JSONTEST_ASSERT_EQUAL(dgJson::uintValue, val.type());

  checks = IsCheck();
  checks.isInt64_ = true;
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asInt64());
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asLargestInt());
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asUInt64());
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asDouble());
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("1099511627776", val.asString());

  // 2^40 (floating-point constructor arg)
  val = dgJson::Value((dgJson::Int64(1) << 40) / 1.0);

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  checks = IsCheck();
  checks.isInt64_ = true;
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asInt64());
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asLargestInt());
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asUInt64());
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asDouble());
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 40), val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL(
      "1099511627776.0",
      normalizeFloatingPointStr(JsonTest::ToJsonString(val.asString())));

  // -2^40
  val = dgJson::Value(-(dgJson::Int64(1) << 40));

  JSONTEST_ASSERT_EQUAL(dgJson::intValue, val.type());

  checks = IsCheck();
  checks.isInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(-(dgJson::Int64(1) << 40), val.asInt64());
  JSONTEST_ASSERT_EQUAL(-(dgJson::Int64(1) << 40), val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(-(dgJson::Int64(1) << 40), val.asDouble());
  JSONTEST_ASSERT_EQUAL(-(dgJson::Int64(1) << 40), val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("-1099511627776", val.asString());

  // int64 max
  val = dgJson::Value(dgJson::Int64(kint64max));

  JSONTEST_ASSERT_EQUAL(dgJson::intValue, val.type());

  checks = IsCheck();
  checks.isInt64_ = true;
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(kint64max, val.asInt64());
  JSONTEST_ASSERT_EQUAL(kint64max, val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(kint64max, val.asUInt64());
  JSONTEST_ASSERT_EQUAL(kint64max, val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(double(kint64max), val.asDouble());
  JSONTEST_ASSERT_EQUAL(float(kint64max), val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("9223372036854775807", val.asString());

  // int64 max (floating point constructor). Note that kint64max is not exactly
  // representable as a double, and will be rounded up to be higher.
  val = dgJson::Value(double(kint64max));

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  checks = IsCheck();
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(dgJson::UInt64(1) << 63, val.asUInt64());
  JSONTEST_ASSERT_EQUAL(dgJson::UInt64(1) << 63, val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(uint64ToDouble(dgJson::UInt64(1) << 63), val.asDouble());
  JSONTEST_ASSERT_EQUAL(float(dgJson::UInt64(1) << 63), val.asFloat());

  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL(
      "9.2233720368547758e+18",
      normalizeFloatingPointStr(JsonTest::ToJsonString(val.asString())));

  // int64 min
  val = dgJson::Value(dgJson::Int64(kint64min));

  JSONTEST_ASSERT_EQUAL(dgJson::intValue, val.type());

  checks = IsCheck();
  checks.isInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(kint64min, val.asInt64());
  JSONTEST_ASSERT_EQUAL(kint64min, val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(double(kint64min), val.asDouble());
  JSONTEST_ASSERT_EQUAL(float(kint64min), val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("-9223372036854775808", val.asString());

  // int64 min (floating point constructor). Note that kint64min *is* exactly
  // representable as a double.
  val = dgJson::Value(double(kint64min));

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  checks = IsCheck();
  checks.isInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(kint64min, val.asInt64());
  JSONTEST_ASSERT_EQUAL(kint64min, val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(-9223372036854775808.0, val.asDouble());
  JSONTEST_ASSERT_EQUAL(-9223372036854775808.0, val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL(
      "-9.2233720368547758e+18",
      normalizeFloatingPointStr(JsonTest::ToJsonString(val.asString())));

  // 10^19
  const dgJson::UInt64 ten_to_19 = static_cast<dgJson::UInt64>(1e19);
  val = dgJson::Value(dgJson::UInt64(ten_to_19));

  JSONTEST_ASSERT_EQUAL(dgJson::uintValue, val.type());

  checks = IsCheck();
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(ten_to_19, val.asUInt64());
  JSONTEST_ASSERT_EQUAL(ten_to_19, val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(uint64ToDouble(ten_to_19), val.asDouble());
  JSONTEST_ASSERT_EQUAL(float(uint64ToDouble(ten_to_19)), val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("10000000000000000000", val.asString());

  // 10^19 (double constructor). Note that 10^19 is not exactly representable
  // as a double.
  val = dgJson::Value(uint64ToDouble(ten_to_19));

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  checks = IsCheck();
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(1e19, val.asDouble());
  JSONTEST_ASSERT_EQUAL(1e19, val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL(
      "1e+19",
      normalizeFloatingPointStr(JsonTest::ToJsonString(val.asString())));

  // uint64 max
  val = dgJson::Value(dgJson::UInt64(kuint64max));

  JSONTEST_ASSERT_EQUAL(dgJson::uintValue, val.type());

  checks = IsCheck();
  checks.isUInt64_ = true;
  checks.isIntegral_ = true;
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(kuint64max, val.asUInt64());
  JSONTEST_ASSERT_EQUAL(kuint64max, val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(uint64ToDouble(kuint64max), val.asDouble());
  JSONTEST_ASSERT_EQUAL(float(uint64ToDouble(kuint64max)), val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL("18446744073709551615", val.asString());

  // uint64 max (floating point constructor). Note that kuint64max is not
  // exactly representable as a double, and will be rounded up to be higher.
  val = dgJson::Value(uint64ToDouble(kuint64max));

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  checks = IsCheck();
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));

  JSONTEST_ASSERT_EQUAL(18446744073709551616.0, val.asDouble());
  JSONTEST_ASSERT_EQUAL(18446744073709551616.0, val.asFloat());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_STRING_EQUAL(
      "1.8446744073709552e+19",
      normalizeFloatingPointStr(JsonTest::ToJsonString(val.asString())));
#endif
}

JSONTEST_FIXTURE(ValueTest, nonIntegers) {
  IsCheck checks;
  dgJson::Value val;

  // Small positive number
  val = dgJson::Value(1.5);

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  checks = IsCheck();
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::uintValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::realValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::booleanValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::stringValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::arrayValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::objectValue));

  JSONTEST_ASSERT_EQUAL(1.5, val.asDouble());
  JSONTEST_ASSERT_EQUAL(1.5, val.asFloat());
  JSONTEST_ASSERT_EQUAL(1, val.asInt());
  JSONTEST_ASSERT_EQUAL(1, val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(1, val.asUInt());
  JSONTEST_ASSERT_EQUAL(1, val.asLargestUInt());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_EQUAL("1.5", val.asString());

  // Small negative number
  val = dgJson::Value(-1.5);

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  checks = IsCheck();
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::realValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::booleanValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::stringValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::arrayValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::objectValue));

  JSONTEST_ASSERT_EQUAL(-1.5, val.asDouble());
  JSONTEST_ASSERT_EQUAL(-1.5, val.asFloat());
  JSONTEST_ASSERT_EQUAL(-1, val.asInt());
  JSONTEST_ASSERT_EQUAL(-1, val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_EQUAL("-1.5", val.asString());

  // A bit over int32 max
  val = dgJson::Value(kint32max + 0.5);

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  checks = IsCheck();
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::uintValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::realValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::booleanValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::stringValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::arrayValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::objectValue));

  JSONTEST_ASSERT_EQUAL(2147483647.5, val.asDouble());
  JSONTEST_ASSERT_EQUAL(float(2147483647.5), val.asFloat());
  JSONTEST_ASSERT_EQUAL(2147483647U, val.asUInt());
#ifdef JSON_HAS_INT64
  JSONTEST_ASSERT_EQUAL(2147483647L, val.asLargestInt());
  JSONTEST_ASSERT_EQUAL(2147483647U, val.asLargestUInt());
#endif
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_EQUAL(
      "2147483647.5",
      normalizeFloatingPointStr(JsonTest::ToJsonString(val.asString())));

  // A bit under int32 min
  val = dgJson::Value(kint32min - 0.5);

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  checks = IsCheck();
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::realValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::booleanValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::stringValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::arrayValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::objectValue));

  JSONTEST_ASSERT_EQUAL(-2147483648.5, val.asDouble());
  JSONTEST_ASSERT_EQUAL(float(-2147483648.5), val.asFloat());
#ifdef JSON_HAS_INT64
  JSONTEST_ASSERT_EQUAL(-(dgJson::Int64(1) << 31), val.asLargestInt());
#endif
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_EQUAL(
      "-2147483648.5",
      normalizeFloatingPointStr(JsonTest::ToJsonString(val.asString())));

  // A bit over uint32 max
  val = dgJson::Value(kuint32max + 0.5);

  JSONTEST_ASSERT_EQUAL(dgJson::realValue, val.type());

  checks = IsCheck();
  checks.isDouble_ = true;
  checks.isNumeric_ = true;
  JSONTEST_ASSERT_PRED(checkIs(val, checks));

  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::realValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::booleanValue));
  JSONTEST_ASSERT(val.isConvertibleTo(dgJson::stringValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::nullValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::intValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::uintValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::arrayValue));
  JSONTEST_ASSERT(!val.isConvertibleTo(dgJson::objectValue));

  JSONTEST_ASSERT_EQUAL(4294967295.5, val.asDouble());
  JSONTEST_ASSERT_EQUAL(float(4294967295.5), val.asFloat());
#ifdef JSON_HAS_INT64
  JSONTEST_ASSERT_EQUAL((dgJson::Int64(1) << 32) - 1, val.asLargestInt());
  JSONTEST_ASSERT_EQUAL((dgJson::UInt64(1) << 32) - dgJson::UInt64(1),
                        val.asLargestUInt());
#endif
  JSONTEST_ASSERT_EQUAL(true, val.asBool());
  JSONTEST_ASSERT_EQUAL(
      "4294967295.5",
      normalizeFloatingPointStr(JsonTest::ToJsonString(val.asString())));

  val = dgJson::Value(1.2345678901234);
  JSONTEST_ASSERT_STRING_EQUAL(
      "1.2345678901234001",
      normalizeFloatingPointStr(JsonTest::ToJsonString(val.asString())));

  // A 16-digit floating point number.
  val = dgJson::Value(2199023255552000.0f);
  JSONTEST_ASSERT_EQUAL(float(2199023255552000.0f), val.asFloat());
  JSONTEST_ASSERT_STRING_EQUAL(
      "2199023255552000.0",
      normalizeFloatingPointStr(JsonTest::ToJsonString(val.asString())));

  // A very large floating point number.
  val = dgJson::Value(3.402823466385289e38);
  JSONTEST_ASSERT_EQUAL(float(3.402823466385289e38), val.asFloat());
  JSONTEST_ASSERT_STRING_EQUAL(
      "3.402823466385289e+38",
      normalizeFloatingPointStr(JsonTest::ToJsonString(val.asString())));

  // An even larger floating point number.
  val = dgJson::Value(1.2345678e300);
  JSONTEST_ASSERT_EQUAL(double(1.2345678e300), val.asDouble());
  JSONTEST_ASSERT_STRING_EQUAL(
      "1.2345678e+300",
      normalizeFloatingPointStr(JsonTest::ToJsonString(val.asString())));
}

void ValueTest::checkConstMemberCount(const dgJson::Value& value,
                                      unsigned int expectedCount) {
  unsigned int count = 0;
  dgJson::Value::const_iterator itEnd = value.end();
  for (dgJson::Value::const_iterator it = value.begin(); it != itEnd; ++it) {
    ++count;
  }
  JSONTEST_ASSERT_EQUAL(expectedCount, count) << "dgJson::Value::const_iterator";
}

void ValueTest::checkMemberCount(dgJson::Value& value,
                                 unsigned int expectedCount) {
  JSONTEST_ASSERT_EQUAL(expectedCount, value.size());

  unsigned int count = 0;
  dgJson::Value::iterator itEnd = value.end();
  for (dgJson::Value::iterator it = value.begin(); it != itEnd; ++it) {
    ++count;
  }
  JSONTEST_ASSERT_EQUAL(expectedCount, count) << "dgJson::Value::iterator";

  JSONTEST_ASSERT_PRED(checkConstMemberCount(value, expectedCount));
}

ValueTest::IsCheck::IsCheck()
    : isObject_(false), isArray_(false), isBool_(false), isString_(false),
      isNull_(false), isInt_(false), isInt64_(false), isUInt_(false),
      isUInt64_(false), isIntegral_(false), isDouble_(false),
      isNumeric_(false) {}

void ValueTest::checkIs(const dgJson::Value& value, const IsCheck& check) {
  JSONTEST_ASSERT_EQUAL(check.isObject_, value.isObject());
  JSONTEST_ASSERT_EQUAL(check.isArray_, value.isArray());
  JSONTEST_ASSERT_EQUAL(check.isBool_, value.isBool());
  JSONTEST_ASSERT_EQUAL(check.isDouble_, value.isDouble());
  JSONTEST_ASSERT_EQUAL(check.isInt_, value.isInt());
  JSONTEST_ASSERT_EQUAL(check.isUInt_, value.isUInt());
  JSONTEST_ASSERT_EQUAL(check.isIntegral_, value.isIntegral());
  JSONTEST_ASSERT_EQUAL(check.isNumeric_, value.isNumeric());
  JSONTEST_ASSERT_EQUAL(check.isString_, value.isString());
  JSONTEST_ASSERT_EQUAL(check.isNull_, value.isNull());

#ifdef JSON_HAS_INT64
  JSONTEST_ASSERT_EQUAL(check.isInt64_, value.isInt64());
  JSONTEST_ASSERT_EQUAL(check.isUInt64_, value.isUInt64());
#else
  JSONTEST_ASSERT_EQUAL(false, value.isInt64());
  JSONTEST_ASSERT_EQUAL(false, value.isUInt64());
#endif
}

JSONTEST_FIXTURE(ValueTest, compareNull) {
  JSONTEST_ASSERT_PRED(checkIsEqual(dgJson::Value(), dgJson::Value()));
}

JSONTEST_FIXTURE(ValueTest, compareInt) {
  JSONTEST_ASSERT_PRED(checkIsLess(0, 10));
  JSONTEST_ASSERT_PRED(checkIsEqual(10, 10));
  JSONTEST_ASSERT_PRED(checkIsEqual(-10, -10));
  JSONTEST_ASSERT_PRED(checkIsLess(-10, 0));
}

JSONTEST_FIXTURE(ValueTest, compareUInt) {
  JSONTEST_ASSERT_PRED(checkIsLess(0u, 10u));
  JSONTEST_ASSERT_PRED(checkIsLess(0u, dgJson::Value::maxUInt));
  JSONTEST_ASSERT_PRED(checkIsEqual(10u, 10u));
}

JSONTEST_FIXTURE(ValueTest, compareDouble) {
  JSONTEST_ASSERT_PRED(checkIsLess(0.0, 10.0));
  JSONTEST_ASSERT_PRED(checkIsEqual(10.0, 10.0));
  JSONTEST_ASSERT_PRED(checkIsEqual(-10.0, -10.0));
  JSONTEST_ASSERT_PRED(checkIsLess(-10.0, 0.0));
}

JSONTEST_FIXTURE(ValueTest, compareString) {
  JSONTEST_ASSERT_PRED(checkIsLess("", " "));
  JSONTEST_ASSERT_PRED(checkIsLess("", "a"));
  JSONTEST_ASSERT_PRED(checkIsLess("abcd", "zyui"));
  JSONTEST_ASSERT_PRED(checkIsLess("abc", "abcd"));
  JSONTEST_ASSERT_PRED(checkIsEqual("abcd", "abcd"));
  JSONTEST_ASSERT_PRED(checkIsEqual(" ", " "));
  JSONTEST_ASSERT_PRED(checkIsLess("ABCD", "abcd"));
  JSONTEST_ASSERT_PRED(checkIsEqual("ABCD", "ABCD"));
}

JSONTEST_FIXTURE(ValueTest, compareBoolean) {
  JSONTEST_ASSERT_PRED(checkIsLess(false, true));
  JSONTEST_ASSERT_PRED(checkIsEqual(false, false));
  JSONTEST_ASSERT_PRED(checkIsEqual(true, true));
}

JSONTEST_FIXTURE(ValueTest, compareArray) {
  // array compare size then content
  dgJson::Value emptyArray(dgJson::arrayValue);
  dgJson::Value l1aArray;
  l1aArray.append(0);
  dgJson::Value l1bArray;
  l1bArray.append(10);
  dgJson::Value l2aArray;
  l2aArray.append(0);
  l2aArray.append(0);
  dgJson::Value l2bArray;
  l2bArray.append(0);
  l2bArray.append(10);
  JSONTEST_ASSERT_PRED(checkIsLess(emptyArray, l1aArray));
  JSONTEST_ASSERT_PRED(checkIsLess(emptyArray, l2aArray));
  JSONTEST_ASSERT_PRED(checkIsLess(l1aArray, l2aArray));
  JSONTEST_ASSERT_PRED(checkIsLess(l2aArray, l2bArray));
  JSONTEST_ASSERT_PRED(checkIsEqual(emptyArray, dgJson::Value(emptyArray)));
  JSONTEST_ASSERT_PRED(checkIsEqual(l1aArray, dgJson::Value(l1aArray)));
  JSONTEST_ASSERT_PRED(checkIsEqual(l2bArray, dgJson::Value(l2bArray)));
}

JSONTEST_FIXTURE(ValueTest, compareObject) {
  // object compare size then content
  dgJson::Value emptyObject(dgJson::objectValue);
  dgJson::Value l1aObject;
  l1aObject["key1"] = 0;
  dgJson::Value l1bObject;
  l1aObject["key1"] = 10;
  dgJson::Value l2aObject;
  l2aObject["key1"] = 0;
  l2aObject["key2"] = 0;
  JSONTEST_ASSERT_PRED(checkIsLess(emptyObject, l1aObject));
  JSONTEST_ASSERT_PRED(checkIsLess(emptyObject, l2aObject));
  JSONTEST_ASSERT_PRED(checkIsLess(l1aObject, l2aObject));
  JSONTEST_ASSERT_PRED(checkIsEqual(emptyObject, dgJson::Value(emptyObject)));
  JSONTEST_ASSERT_PRED(checkIsEqual(l1aObject, dgJson::Value(l1aObject)));
  JSONTEST_ASSERT_PRED(checkIsEqual(l2aObject, dgJson::Value(l2aObject)));
}

JSONTEST_FIXTURE(ValueTest, compareType) {
  // object of different type are ordered according to their type
  JSONTEST_ASSERT_PRED(checkIsLess(dgJson::Value(), dgJson::Value(1)));
  JSONTEST_ASSERT_PRED(checkIsLess(dgJson::Value(1), dgJson::Value(1u)));
  JSONTEST_ASSERT_PRED(checkIsLess(dgJson::Value(1u), dgJson::Value(1.0)));
  JSONTEST_ASSERT_PRED(checkIsLess(dgJson::Value(1.0), dgJson::Value("a")));
  JSONTEST_ASSERT_PRED(checkIsLess(dgJson::Value("a"), dgJson::Value(true)));
  JSONTEST_ASSERT_PRED(
      checkIsLess(dgJson::Value(true), dgJson::Value(dgJson::arrayValue)));
  JSONTEST_ASSERT_PRED(checkIsLess(dgJson::Value(dgJson::arrayValue),
                                   dgJson::Value(dgJson::objectValue)));
}

JSONTEST_FIXTURE(ValueTest, CopyObject) {
  dgJson::Value arrayVal;
  arrayVal.append("val1");
  arrayVal.append("val2");
  arrayVal.append("val3");
  dgJson::Value stringVal("string value");
  dgJson::Value copy1, copy2;
  {
    dgJson::Value arrayCopy, stringCopy;
    arrayCopy.copy(arrayVal);
    stringCopy.copy(stringVal);
    JSONTEST_ASSERT_PRED(checkIsEqual(arrayCopy, arrayVal));
    JSONTEST_ASSERT_PRED(checkIsEqual(stringCopy, stringVal));
    arrayCopy.append("val4");
    JSONTEST_ASSERT(arrayCopy.size() == 4);
    arrayVal.append("new4");
    arrayVal.append("new5");
    JSONTEST_ASSERT(arrayVal.size() == 5);
    JSONTEST_ASSERT(!(arrayCopy == arrayVal));
    stringCopy = "another string";
    JSONTEST_ASSERT(!(stringCopy == stringVal));
    copy1.copy(arrayCopy);
    copy2.copy(stringCopy);
  }
  JSONTEST_ASSERT(arrayVal.size() == 5);
  JSONTEST_ASSERT(stringVal == "string value");
  JSONTEST_ASSERT(copy1.size() == 4);
  JSONTEST_ASSERT(copy2 == "another string");
  copy1.copy(stringVal);
  JSONTEST_ASSERT(copy1 == "string value");
  copy2.copy(arrayVal);
  JSONTEST_ASSERT(copy2.size() == 5);
}

void ValueTest::checkIsLess(const dgJson::Value& x, const dgJson::Value& y) {
  JSONTEST_ASSERT(x < y);
  JSONTEST_ASSERT(y > x);
  JSONTEST_ASSERT(x <= y);
  JSONTEST_ASSERT(y >= x);
  JSONTEST_ASSERT(!(x == y));
  JSONTEST_ASSERT(!(y == x));
  JSONTEST_ASSERT(!(x >= y));
  JSONTEST_ASSERT(!(y <= x));
  JSONTEST_ASSERT(!(x > y));
  JSONTEST_ASSERT(!(y < x));
  JSONTEST_ASSERT(x.compare(y) < 0);
  JSONTEST_ASSERT(y.compare(x) >= 0);
}

void ValueTest::checkIsEqual(const dgJson::Value& x, const dgJson::Value& y) {
  JSONTEST_ASSERT(x == y);
  JSONTEST_ASSERT(y == x);
  JSONTEST_ASSERT(x <= y);
  JSONTEST_ASSERT(y <= x);
  JSONTEST_ASSERT(x >= y);
  JSONTEST_ASSERT(y >= x);
  JSONTEST_ASSERT(!(x < y));
  JSONTEST_ASSERT(!(y < x));
  JSONTEST_ASSERT(!(x > y));
  JSONTEST_ASSERT(!(y > x));
  JSONTEST_ASSERT(x.compare(y) == 0);
  JSONTEST_ASSERT(y.compare(x) == 0);
}

JSONTEST_FIXTURE(ValueTest, typeChecksThrowExceptions) {
#if JSON_USE_EXCEPTION

  dgJson::Value intVal(1);
  dgJson::Value strVal("Test");
  dgJson::Value objVal(dgJson::objectValue);
  dgJson::Value arrVal(dgJson::arrayValue);

  JSONTEST_ASSERT_THROWS(intVal["test"]);
  JSONTEST_ASSERT_THROWS(strVal["test"]);
  JSONTEST_ASSERT_THROWS(arrVal["test"]);

  JSONTEST_ASSERT_THROWS(intVal.removeMember("test"));
  JSONTEST_ASSERT_THROWS(strVal.removeMember("test"));
  JSONTEST_ASSERT_THROWS(arrVal.removeMember("test"));

  JSONTEST_ASSERT_THROWS(intVal.getMemberNames());
  JSONTEST_ASSERT_THROWS(strVal.getMemberNames());
  JSONTEST_ASSERT_THROWS(arrVal.getMemberNames());

  JSONTEST_ASSERT_THROWS(intVal[0]);
  JSONTEST_ASSERT_THROWS(objVal[0]);
  JSONTEST_ASSERT_THROWS(strVal[0]);

  JSONTEST_ASSERT_THROWS(intVal.clear());

  JSONTEST_ASSERT_THROWS(intVal.resize(1));
  JSONTEST_ASSERT_THROWS(strVal.resize(1));
  JSONTEST_ASSERT_THROWS(objVal.resize(1));

  JSONTEST_ASSERT_THROWS(intVal.asCString());

  JSONTEST_ASSERT_THROWS(objVal.asString());
  JSONTEST_ASSERT_THROWS(arrVal.asString());

  JSONTEST_ASSERT_THROWS(strVal.asInt());
  JSONTEST_ASSERT_THROWS(objVal.asInt());
  JSONTEST_ASSERT_THROWS(arrVal.asInt());

  JSONTEST_ASSERT_THROWS(strVal.asUInt());
  JSONTEST_ASSERT_THROWS(objVal.asUInt());
  JSONTEST_ASSERT_THROWS(arrVal.asUInt());

  JSONTEST_ASSERT_THROWS(strVal.asInt64());
  JSONTEST_ASSERT_THROWS(objVal.asInt64());
  JSONTEST_ASSERT_THROWS(arrVal.asInt64());

  JSONTEST_ASSERT_THROWS(strVal.asUInt64());
  JSONTEST_ASSERT_THROWS(objVal.asUInt64());
  JSONTEST_ASSERT_THROWS(arrVal.asUInt64());

  JSONTEST_ASSERT_THROWS(strVal.asDouble());
  JSONTEST_ASSERT_THROWS(objVal.asDouble());
  JSONTEST_ASSERT_THROWS(arrVal.asDouble());

  JSONTEST_ASSERT_THROWS(strVal.asFloat());
  JSONTEST_ASSERT_THROWS(objVal.asFloat());
  JSONTEST_ASSERT_THROWS(arrVal.asFloat());

  JSONTEST_ASSERT_THROWS(strVal.asBool());
  JSONTEST_ASSERT_THROWS(objVal.asBool());
  JSONTEST_ASSERT_THROWS(arrVal.asBool());

#endif
}

JSONTEST_FIXTURE(ValueTest, offsetAccessors) {
  dgJson::Value x;
  JSONTEST_ASSERT(x.getOffsetStart() == 0);
  JSONTEST_ASSERT(x.getOffsetLimit() == 0);
  x.setOffsetStart(10);
  x.setOffsetLimit(20);
  JSONTEST_ASSERT(x.getOffsetStart() == 10);
  JSONTEST_ASSERT(x.getOffsetLimit() == 20);
  dgJson::Value y(x);
  JSONTEST_ASSERT(y.getOffsetStart() == 10);
  JSONTEST_ASSERT(y.getOffsetLimit() == 20);
  dgJson::Value z;
  z.swap(y);
  JSONTEST_ASSERT(z.getOffsetStart() == 10);
  JSONTEST_ASSERT(z.getOffsetLimit() == 20);
  JSONTEST_ASSERT(y.getOffsetStart() == 0);
  JSONTEST_ASSERT(y.getOffsetLimit() == 0);
}

JSONTEST_FIXTURE(ValueTest, StaticString) {
  char mutant[] = "hello";
  dgJson::StaticString ss(mutant);
  JSONCPP_STRING regular(mutant);
  mutant[1] = 'a';
  JSONTEST_ASSERT_STRING_EQUAL("hallo", ss.c_str());
  JSONTEST_ASSERT_STRING_EQUAL("hello", regular.c_str());
  {
    dgJson::Value root;
    root["top"] = ss;
    JSONTEST_ASSERT_STRING_EQUAL("hallo", root["top"].asString());
    mutant[1] = 'u';
    JSONTEST_ASSERT_STRING_EQUAL("hullo", root["top"].asString());
  }
  {
    dgJson::Value root;
    root["top"] = regular;
    JSONTEST_ASSERT_STRING_EQUAL("hello", root["top"].asString());
    mutant[1] = 'u';
    JSONTEST_ASSERT_STRING_EQUAL("hello", root["top"].asString());
  }
}

JSONTEST_FIXTURE(ValueTest, CommentBefore) {
  dgJson::Value val; // fill val
  val.setComment(JSONCPP_STRING("// this comment should appear before"),
                 dgJson::commentBefore);
  dgJson::StreamWriterBuilder wbuilder;
  wbuilder.settings_["commentStyle"] = "All";
  {
    char const expected[] = "// this comment should appear before\nnull";
    JSONCPP_STRING result = dgJson::writeString(wbuilder, val);
    JSONTEST_ASSERT_STRING_EQUAL(expected, result);
    JSONCPP_STRING res2 = val.toStyledString();
    JSONCPP_STRING exp2 = "\n";
    exp2 += expected;
    exp2 += "\n";
    JSONTEST_ASSERT_STRING_EQUAL(exp2, res2);
  }
  dgJson::Value other = "hello";
  val.swapPayload(other);
  {
    char const expected[] = "// this comment should appear before\n\"hello\"";
    JSONCPP_STRING result = dgJson::writeString(wbuilder, val);
    JSONTEST_ASSERT_STRING_EQUAL(expected, result);
    JSONCPP_STRING res2 = val.toStyledString();
    JSONCPP_STRING exp2 = "\n";
    exp2 += expected;
    exp2 += "\n";
    JSONTEST_ASSERT_STRING_EQUAL(exp2, res2);
    JSONTEST_ASSERT_STRING_EQUAL("null\n", other.toStyledString());
  }
  val = "hello";
  // val.setComment("// this comment should appear before",
  // dgJson::CommentPlacement::commentBefore); Assignment over-writes comments.
  {
    char const expected[] = "\"hello\"";
    JSONCPP_STRING result = dgJson::writeString(wbuilder, val);
    JSONTEST_ASSERT_STRING_EQUAL(expected, result);
    JSONCPP_STRING res2 = val.toStyledString();
    JSONCPP_STRING exp2 = "";
    exp2 += expected;
    exp2 += "\n";
    JSONTEST_ASSERT_STRING_EQUAL(exp2, res2);
  }
}

JSONTEST_FIXTURE(ValueTest, zeroes) {
  char const cstr[] = "h\0i";
  JSONCPP_STRING binary(cstr, sizeof(cstr)); // include trailing 0
  JSONTEST_ASSERT_EQUAL(4U, binary.length());
  dgJson::StreamWriterBuilder b;
  {
    dgJson::Value root;
    root = binary;
    JSONTEST_ASSERT_STRING_EQUAL(binary, root.asString());
  }
  {
    char const top[] = "top";
    dgJson::Value root;
    root[top] = binary;
    JSONTEST_ASSERT_STRING_EQUAL(binary, root[top].asString());
    dgJson::Value removed;
    bool did;
    did = root.removeMember(top, top + sizeof(top) - 1U, &removed);
    JSONTEST_ASSERT(did);
    JSONTEST_ASSERT_STRING_EQUAL(binary, removed.asString());
    did = root.removeMember(top, top + sizeof(top) - 1U, &removed);
    JSONTEST_ASSERT(!did);
    JSONTEST_ASSERT_STRING_EQUAL(binary, removed.asString()); // still
  }
}

JSONTEST_FIXTURE(ValueTest, zeroesInKeys) {
  char const cstr[] = "h\0i";
  JSONCPP_STRING binary(cstr, sizeof(cstr)); // include trailing 0
  JSONTEST_ASSERT_EQUAL(4U, binary.length());
  {
    dgJson::Value root;
    root[binary] = "there";
    JSONTEST_ASSERT_STRING_EQUAL("there", root[binary].asString());
    JSONTEST_ASSERT(!root.isMember("h"));
    JSONTEST_ASSERT(root.isMember(binary));
    JSONTEST_ASSERT_STRING_EQUAL(
        "there", root.get(binary, dgJson::Value::nullRef).asString());
    dgJson::Value removed;
    bool did;
    did = root.removeMember(binary.data(), binary.data() + binary.length(),
                            &removed);
    JSONTEST_ASSERT(did);
    JSONTEST_ASSERT_STRING_EQUAL("there", removed.asString());
    did = root.removeMember(binary.data(), binary.data() + binary.length(),
                            &removed);
    JSONTEST_ASSERT(!did);
    JSONTEST_ASSERT_STRING_EQUAL("there", removed.asString()); // still
    JSONTEST_ASSERT(!root.isMember(binary));
    JSONTEST_ASSERT_STRING_EQUAL(
        "", root.get(binary, dgJson::Value::nullRef).asString());
  }
}

JSONTEST_FIXTURE(ValueTest, specialFloats) {
  dgJson::StreamWriterBuilder b;
  b.settings_["useSpecialFloats"] = true;

  dgJson::Value v = std::numeric_limits<double>::quiet_NaN();
  JSONCPP_STRING expected = "NaN";
  JSONCPP_STRING result = dgJson::writeString(b, v);
  JSONTEST_ASSERT_STRING_EQUAL(expected, result);

  v = std::numeric_limits<double>::infinity();
  expected = "Infinity";
  result = dgJson::writeString(b, v);
  JSONTEST_ASSERT_STRING_EQUAL(expected, result);

  v = -std::numeric_limits<double>::infinity();
  expected = "-Infinity";
  result = dgJson::writeString(b, v);
  JSONTEST_ASSERT_STRING_EQUAL(expected, result);
}

JSONTEST_FIXTURE(ValueTest, precision) {
  dgJson::StreamWriterBuilder b;
  b.settings_["precision"] = 5;

  dgJson::Value v = 100.0 / 3;
  JSONCPP_STRING expected = "33.333";
  JSONCPP_STRING result = dgJson::writeString(b, v);
  JSONTEST_ASSERT_STRING_EQUAL(expected, result);

  v = 0.25000000;
  expected = "0.25";
  result = dgJson::writeString(b, v);
  JSONTEST_ASSERT_STRING_EQUAL(expected, result);

  v = 0.2563456;
  expected = "0.25635";
  result = dgJson::writeString(b, v);
  JSONTEST_ASSERT_STRING_EQUAL(expected, result);

  b.settings_["precision"] = 1;
  expected = "0.3";
  result = dgJson::writeString(b, v);
  JSONTEST_ASSERT_STRING_EQUAL(expected, result);

  b.settings_["precision"] = 17;
  v = 1234857476305.256345694873740545068;
  expected = "1234857476305.2563";
  result = dgJson::writeString(b, v);
  JSONTEST_ASSERT_STRING_EQUAL(expected, result);

  b.settings_["precision"] = 24;
  v = 0.256345694873740545068;
  expected = "0.25634569487374054";
  result = dgJson::writeString(b, v);
  JSONTEST_ASSERT_STRING_EQUAL(expected, result);

  b.settings_["precision"] = 5;
  b.settings_["precisionType"] = "decimal";
  v = 0.256345694873740545068;
  expected = "0.25635";
  result = dgJson::writeString(b, v);
  JSONTEST_ASSERT_STRING_EQUAL(expected, result);

  b.settings_["precision"] = 1;
  b.settings_["precisionType"] = "decimal";
  v = 0.256345694873740545068;
  expected = "0.3";
  result = dgJson::writeString(b, v);
  JSONTEST_ASSERT_STRING_EQUAL(expected, result);

  b.settings_["precision"] = 10;
  b.settings_["precisionType"] = "decimal";
  v = 0.23300000;
  expected = "0.233";
  result = dgJson::writeString(b, v);
  JSONTEST_ASSERT_STRING_EQUAL(expected, result);
}

struct WriterTest : JsonTest::TestCase {};

JSONTEST_FIXTURE(WriterTest, dropNullPlaceholders) {
  dgJson::FastWriter writer;
  dgJson::Value nullValue;
  JSONTEST_ASSERT(writer.write(nullValue) == "null\n");

  writer.dropNullPlaceholders();
  JSONTEST_ASSERT(writer.write(nullValue) == "\n");
}

struct StreamWriterTest : JsonTest::TestCase {};

JSONTEST_FIXTURE(StreamWriterTest, dropNullPlaceholders) {
  dgJson::StreamWriterBuilder b;
  dgJson::Value nullValue;
  b.settings_["dropNullPlaceholders"] = false;
  JSONTEST_ASSERT(dgJson::writeString(b, nullValue) == "null");
  b.settings_["dropNullPlaceholders"] = true;
  JSONTEST_ASSERT(dgJson::writeString(b, nullValue) == "");
}

JSONTEST_FIXTURE(StreamWriterTest, writeZeroes) {
  JSONCPP_STRING binary("hi", 3); // include trailing 0
  JSONTEST_ASSERT_EQUAL(3, binary.length());
  JSONCPP_STRING expected("\"hi\\u0000\""); // unicoded zero
  dgJson::StreamWriterBuilder b;
  {
    dgJson::Value root;
    root = binary;
    JSONTEST_ASSERT_STRING_EQUAL(binary, root.asString());
    JSONCPP_STRING out = dgJson::writeString(b, root);
    JSONTEST_ASSERT_EQUAL(expected.size(), out.size());
    JSONTEST_ASSERT_STRING_EQUAL(expected, out);
  }
  {
    dgJson::Value root;
    root["top"] = binary;
    JSONTEST_ASSERT_STRING_EQUAL(binary, root["top"].asString());
    JSONCPP_STRING out = dgJson::writeString(b, root["top"]);
    JSONTEST_ASSERT_STRING_EQUAL(expected, out);
  }
}

struct ReaderTest : JsonTest::TestCase {};

JSONTEST_FIXTURE(ReaderTest, parseWithNoErrors) {
  dgJson::Reader reader;
  dgJson::Value root;
  bool ok = reader.parse("{ \"property\" : \"value\" }", root);
  JSONTEST_ASSERT(ok);
  JSONTEST_ASSERT(reader.getFormattedErrorMessages().size() == 0);
  JSONTEST_ASSERT(reader.getStructuredErrors().size() == 0);
}

JSONTEST_FIXTURE(ReaderTest, parseWithNoErrorsTestingOffsets) {
  dgJson::Reader reader;
  dgJson::Value root;
  bool ok = reader.parse("{ \"property\" : [\"value\", \"value2\"], \"obj\" : "
                         "{ \"nested\" : 123, \"bool\" : true}, \"null\" : "
                         "null, \"false\" : false }",
                         root);
  JSONTEST_ASSERT(ok);
  JSONTEST_ASSERT(reader.getFormattedErrorMessages().size() == 0);
  JSONTEST_ASSERT(reader.getStructuredErrors().size() == 0);
  JSONTEST_ASSERT(root["property"].getOffsetStart() == 15);
  JSONTEST_ASSERT(root["property"].getOffsetLimit() == 34);
  JSONTEST_ASSERT(root["property"][0].getOffsetStart() == 16);
  JSONTEST_ASSERT(root["property"][0].getOffsetLimit() == 23);
  JSONTEST_ASSERT(root["property"][1].getOffsetStart() == 25);
  JSONTEST_ASSERT(root["property"][1].getOffsetLimit() == 33);
  JSONTEST_ASSERT(root["obj"].getOffsetStart() == 44);
  JSONTEST_ASSERT(root["obj"].getOffsetLimit() == 76);
  JSONTEST_ASSERT(root["obj"]["nested"].getOffsetStart() == 57);
  JSONTEST_ASSERT(root["obj"]["nested"].getOffsetLimit() == 60);
  JSONTEST_ASSERT(root["obj"]["bool"].getOffsetStart() == 71);
  JSONTEST_ASSERT(root["obj"]["bool"].getOffsetLimit() == 75);
  JSONTEST_ASSERT(root["null"].getOffsetStart() == 87);
  JSONTEST_ASSERT(root["null"].getOffsetLimit() == 91);
  JSONTEST_ASSERT(root["false"].getOffsetStart() == 103);
  JSONTEST_ASSERT(root["false"].getOffsetLimit() == 108);
  JSONTEST_ASSERT(root.getOffsetStart() == 0);
  JSONTEST_ASSERT(root.getOffsetLimit() == 110);
}

JSONTEST_FIXTURE(ReaderTest, parseWithOneError) {
  dgJson::Reader reader;
  dgJson::Value root;
  bool ok = reader.parse("{ \"property\" :: \"value\" }", root);
  JSONTEST_ASSERT(!ok);
  JSONTEST_ASSERT(reader.getFormattedErrorMessages() ==
                  "* Line 1, Column 15\n  Syntax error: value, object or array "
                  "expected.\n");
  std::vector<dgJson::Reader::StructuredError> errors =
      reader.getStructuredErrors();
  JSONTEST_ASSERT(errors.size() == 1);
  JSONTEST_ASSERT(errors.at(0).offset_start == 14);
  JSONTEST_ASSERT(errors.at(0).offset_limit == 15);
  JSONTEST_ASSERT(errors.at(0).message ==
                  "Syntax error: value, object or array expected.");
}

JSONTEST_FIXTURE(ReaderTest, parseChineseWithOneError) {
  dgJson::Reader reader;
  dgJson::Value root;
  bool ok = reader.parse("{ \"pr佐藤erty\" :: \"value\" }", root);
  JSONTEST_ASSERT(!ok);
  JSONTEST_ASSERT(reader.getFormattedErrorMessages() ==
                  "* Line 1, Column 19\n  Syntax error: value, object or array "
                  "expected.\n");
  std::vector<dgJson::Reader::StructuredError> errors =
      reader.getStructuredErrors();
  JSONTEST_ASSERT(errors.size() == 1);
  JSONTEST_ASSERT(errors.at(0).offset_start == 18);
  JSONTEST_ASSERT(errors.at(0).offset_limit == 19);
  JSONTEST_ASSERT(errors.at(0).message ==
                  "Syntax error: value, object or array expected.");
}

JSONTEST_FIXTURE(ReaderTest, parseWithDetailError) {
  dgJson::Reader reader;
  dgJson::Value root;
  bool ok = reader.parse("{ \"property\" : \"v\\alue\" }", root);
  JSONTEST_ASSERT(!ok);
  JSONTEST_ASSERT(reader.getFormattedErrorMessages() ==
                  "* Line 1, Column 16\n  Bad escape sequence in string\nSee "
                  "Line 1, Column 20 for detail.\n");
  std::vector<dgJson::Reader::StructuredError> errors =
      reader.getStructuredErrors();
  JSONTEST_ASSERT(errors.size() == 1);
  JSONTEST_ASSERT(errors.at(0).offset_start == 15);
  JSONTEST_ASSERT(errors.at(0).offset_limit == 23);
  JSONTEST_ASSERT(errors.at(0).message == "Bad escape sequence in string");
}

struct CharReaderTest : JsonTest::TestCase {};

JSONTEST_FIXTURE(CharReaderTest, parseWithNoErrors) {
  dgJson::CharReaderBuilder b;
  dgJson::CharReader* reader(b.newCharReader());
  JSONCPP_STRING errs;
  dgJson::Value root;
  char const doc[] = "{ \"property\" : \"value\" }";
  bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
  JSONTEST_ASSERT(ok);
  JSONTEST_ASSERT(errs.size() == 0);
  delete reader;
}

JSONTEST_FIXTURE(CharReaderTest, parseWithNoErrorsTestingOffsets) {
  dgJson::CharReaderBuilder b;
  dgJson::CharReader* reader(b.newCharReader());
  JSONCPP_STRING errs;
  dgJson::Value root;
  char const doc[] = "{ \"property\" : [\"value\", \"value2\"], \"obj\" : "
                     "{ \"nested\" : 123, \"bool\" : true}, \"null\" : "
                     "null, \"false\" : false }";
  bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
  JSONTEST_ASSERT(ok);
  JSONTEST_ASSERT(errs.size() == 0);
  delete reader;
}

JSONTEST_FIXTURE(CharReaderTest, parseWithOneError) {
  dgJson::CharReaderBuilder b;
  dgJson::CharReader* reader(b.newCharReader());
  JSONCPP_STRING errs;
  dgJson::Value root;
  char const doc[] = "{ \"property\" :: \"value\" }";
  bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
  JSONTEST_ASSERT(!ok);
  JSONTEST_ASSERT(errs ==
                  "* Line 1, Column 15\n  Syntax error: value, object or array "
                  "expected.\n");
  delete reader;
}

JSONTEST_FIXTURE(CharReaderTest, parseChineseWithOneError) {
  dgJson::CharReaderBuilder b;
  dgJson::CharReader* reader(b.newCharReader());
  JSONCPP_STRING errs;
  dgJson::Value root;
  char const doc[] = "{ \"pr佐藤erty\" :: \"value\" }";
  bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
  JSONTEST_ASSERT(!ok);
  JSONTEST_ASSERT(errs ==
                  "* Line 1, Column 19\n  Syntax error: value, object or array "
                  "expected.\n");
  delete reader;
}

JSONTEST_FIXTURE(CharReaderTest, parseWithDetailError) {
  dgJson::CharReaderBuilder b;
  dgJson::CharReader* reader(b.newCharReader());
  JSONCPP_STRING errs;
  dgJson::Value root;
  char const doc[] = "{ \"property\" : \"v\\alue\" }";
  bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
  JSONTEST_ASSERT(!ok);
  JSONTEST_ASSERT(errs ==
                  "* Line 1, Column 16\n  Bad escape sequence in string\nSee "
                  "Line 1, Column 20 for detail.\n");
  delete reader;
}

JSONTEST_FIXTURE(CharReaderTest, parseWithStackLimit) {
  dgJson::CharReaderBuilder b;
  dgJson::Value root;
  char const doc[] = "{ \"property\" : \"value\" }";
  {
    b.settings_["stackLimit"] = 2;
    dgJson::CharReader* reader(b.newCharReader());
    JSONCPP_STRING errs;
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT(errs == "");
    JSONTEST_ASSERT_EQUAL("value", root["property"]);
    delete reader;
  }
  {
    b.settings_["stackLimit"] = 1;
    dgJson::CharReader* reader(b.newCharReader());
    JSONCPP_STRING errs;
    JSONTEST_ASSERT_THROWS(
        reader->parse(doc, doc + std::strlen(doc), &root, &errs));
    delete reader;
  }
}

struct CharReaderStrictModeTest : JsonTest::TestCase {};

JSONTEST_FIXTURE(CharReaderStrictModeTest, dupKeys) {
  dgJson::CharReaderBuilder b;
  dgJson::Value root;
  char const doc[] =
      "{ \"property\" : \"value\", \"key\" : \"val1\", \"key\" : \"val2\" }";
  {
    b.strictMode(&b.settings_);
    dgJson::CharReader* reader(b.newCharReader());
    JSONCPP_STRING errs;
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(!ok);
    JSONTEST_ASSERT_STRING_EQUAL("* Line 1, Column 41\n"
                                 "  Duplicate key: 'key'\n",
                                 errs);
    JSONTEST_ASSERT_EQUAL("val1", root["key"]); // so far
    delete reader;
  }
}
struct CharReaderFailIfExtraTest : JsonTest::TestCase {};

JSONTEST_FIXTURE(CharReaderFailIfExtraTest, issue164) {
  // This is interpreted as a string value followed by a colon.
  dgJson::CharReaderBuilder b;
  dgJson::Value root;
  char const doc[] = " \"property\" : \"value\" }";
  {
    b.settings_["failIfExtra"] = false;
    dgJson::CharReader* reader(b.newCharReader());
    JSONCPP_STRING errs;
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT(errs == "");
    JSONTEST_ASSERT_EQUAL("property", root);
    delete reader;
  }
  {
    b.settings_["failIfExtra"] = true;
    dgJson::CharReader* reader(b.newCharReader());
    JSONCPP_STRING errs;
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(!ok);
    JSONTEST_ASSERT_STRING_EQUAL(errs,
                                 "* Line 1, Column 13\n"
                                 "  Extra non-whitespace after JSON value.\n");
    JSONTEST_ASSERT_EQUAL("property", root);
    delete reader;
  }
  {
    b.settings_["failIfExtra"] = false;
    b.strictMode(&b.settings_);
    dgJson::CharReader* reader(b.newCharReader());
    JSONCPP_STRING errs;
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(!ok);
    JSONTEST_ASSERT_STRING_EQUAL(errs,
                                 "* Line 1, Column 13\n"
                                 "  Extra non-whitespace after JSON value.\n");
    JSONTEST_ASSERT_EQUAL("property", root);
    delete reader;
  }
}
JSONTEST_FIXTURE(CharReaderFailIfExtraTest, issue107) {
  // This is interpreted as an int value followed by a colon.
  dgJson::CharReaderBuilder b;
  dgJson::Value root;
  char const doc[] = "1:2:3";
  b.settings_["failIfExtra"] = true;
  dgJson::CharReader* reader(b.newCharReader());
  JSONCPP_STRING errs;
  bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
  JSONTEST_ASSERT(!ok);
  JSONTEST_ASSERT_STRING_EQUAL("* Line 1, Column 2\n"
                               "  Extra non-whitespace after JSON value.\n",
                               errs);
  JSONTEST_ASSERT_EQUAL(1, root.asInt());
  delete reader;
}
JSONTEST_FIXTURE(CharReaderFailIfExtraTest, commentAfterObject) {
  dgJson::CharReaderBuilder b;
  dgJson::Value root;
  {
    char const doc[] = "{ \"property\" : \"value\" } //trailing\n//comment\n";
    b.settings_["failIfExtra"] = true;
    dgJson::CharReader* reader(b.newCharReader());
    JSONCPP_STRING errs;
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL("value", root["property"]);
    delete reader;
  }
}
JSONTEST_FIXTURE(CharReaderFailIfExtraTest, commentAfterArray) {
  dgJson::CharReaderBuilder b;
  dgJson::Value root;
  char const doc[] = "[ \"property\" , \"value\" ] //trailing\n//comment\n";
  b.settings_["failIfExtra"] = true;
  dgJson::CharReader* reader(b.newCharReader());
  JSONCPP_STRING errs;
  bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
  JSONTEST_ASSERT(ok);
  JSONTEST_ASSERT_STRING_EQUAL("", errs);
  JSONTEST_ASSERT_EQUAL("value", root[1u]);
  delete reader;
}
JSONTEST_FIXTURE(CharReaderFailIfExtraTest, commentAfterBool) {
  dgJson::CharReaderBuilder b;
  dgJson::Value root;
  char const doc[] = " true /*trailing\ncomment*/";
  b.settings_["failIfExtra"] = true;
  dgJson::CharReader* reader(b.newCharReader());
  JSONCPP_STRING errs;
  bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
  JSONTEST_ASSERT(ok);
  JSONTEST_ASSERT_STRING_EQUAL("", errs);
  JSONTEST_ASSERT_EQUAL(true, root.asBool());
  delete reader;
}
struct CharReaderAllowDropNullTest : JsonTest::TestCase {};

JSONTEST_FIXTURE(CharReaderAllowDropNullTest, issue178) {
  dgJson::CharReaderBuilder b;
  b.settings_["allowDroppedNullPlaceholders"] = true;
  dgJson::Value root;
  JSONCPP_STRING errs;
  dgJson::CharReader* reader(b.newCharReader());
  {
    char const doc[] = "{\"a\":,\"b\":true}";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(2u, root.size());
    JSONTEST_ASSERT_EQUAL(dgJson::nullValue, root.get("a", true));
  }
  {
    char const doc[] = "{\"a\":}";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(1u, root.size());
    JSONTEST_ASSERT_EQUAL(dgJson::nullValue, root.get("a", true));
  }
  {
    char const doc[] = "[]";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT(errs == "");
    JSONTEST_ASSERT_EQUAL(0u, root.size());
    JSONTEST_ASSERT_EQUAL(dgJson::arrayValue, root);
  }
  {
    char const doc[] = "[null]";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT(errs == "");
    JSONTEST_ASSERT_EQUAL(1u, root.size());
  }
  {
    char const doc[] = "[,]";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(2u, root.size());
  }
  {
    char const doc[] = "[,,,]";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(4u, root.size());
  }
  {
    char const doc[] = "[null,]";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(2u, root.size());
  }
  {
    char const doc[] = "[,null]";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT(errs == "");
    JSONTEST_ASSERT_EQUAL(2u, root.size());
  }
  {
    char const doc[] = "[,,]";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(3u, root.size());
  }
  {
    char const doc[] = "[null,,]";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(3u, root.size());
  }
  {
    char const doc[] = "[,null,]";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(3u, root.size());
  }
  {
    char const doc[] = "[,,null]";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT(errs == "");
    JSONTEST_ASSERT_EQUAL(3u, root.size());
  }
  {
    char const doc[] = "[[],,,]";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(4u, root.size());
    JSONTEST_ASSERT_EQUAL(dgJson::arrayValue, root[0u]);
  }
  {
    char const doc[] = "[,[],,]";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(4u, root.size());
    JSONTEST_ASSERT_EQUAL(dgJson::arrayValue, root[1u]);
  }
  {
    char const doc[] = "[,,,[]]";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT(errs == "");
    JSONTEST_ASSERT_EQUAL(4u, root.size());
    JSONTEST_ASSERT_EQUAL(dgJson::arrayValue, root[3u]);
  }
  delete reader;
}

struct CharReaderAllowSingleQuotesTest : JsonTest::TestCase {};

JSONTEST_FIXTURE(CharReaderAllowSingleQuotesTest, issue182) {
  dgJson::CharReaderBuilder b;
  b.settings_["allowSingleQuotes"] = true;
  dgJson::Value root;
  JSONCPP_STRING errs;
  dgJson::CharReader* reader(b.newCharReader());
  {
    char const doc[] = "{'a':true,\"b\":true}";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(2u, root.size());
    JSONTEST_ASSERT_EQUAL(true, root.get("a", false));
    JSONTEST_ASSERT_EQUAL(true, root.get("b", false));
  }
  {
    char const doc[] = "{'a': 'x', \"b\":'y'}";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(2u, root.size());
    JSONTEST_ASSERT_STRING_EQUAL("x", root["a"].asString());
    JSONTEST_ASSERT_STRING_EQUAL("y", root["b"].asString());
  }
  delete reader;
}

struct CharReaderAllowZeroesTest : JsonTest::TestCase {};

JSONTEST_FIXTURE(CharReaderAllowZeroesTest, issue176) {
  dgJson::CharReaderBuilder b;
  b.settings_["allowSingleQuotes"] = true;
  dgJson::Value root;
  JSONCPP_STRING errs;
  dgJson::CharReader* reader(b.newCharReader());
  {
    char const doc[] = "{'a':true,\"b\":true}";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(2u, root.size());
    JSONTEST_ASSERT_EQUAL(true, root.get("a", false));
    JSONTEST_ASSERT_EQUAL(true, root.get("b", false));
  }
  {
    char const doc[] = "{'a': 'x', \"b\":'y'}";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(2u, root.size());
    JSONTEST_ASSERT_STRING_EQUAL("x", root["a"].asString());
    JSONTEST_ASSERT_STRING_EQUAL("y", root["b"].asString());
  }
  delete reader;
}

struct CharReaderAllowSpecialFloatsTest : JsonTest::TestCase {};

JSONTEST_FIXTURE(CharReaderAllowSpecialFloatsTest, issue209) {
  dgJson::CharReaderBuilder b;
  b.settings_["allowSpecialFloats"] = true;
  dgJson::Value root;
  JSONCPP_STRING errs;
  dgJson::CharReader* reader(b.newCharReader());
  {
    char const doc[] = "{\"a\":NaN,\"b\":Infinity,\"c\":-Infinity}";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(3u, root.size());
    double n = root["a"].asDouble();
    JSONTEST_ASSERT(std::isnan(n));
    JSONTEST_ASSERT_EQUAL(std::numeric_limits<double>::infinity(),
                          root.get("b", 0.0));
    JSONTEST_ASSERT_EQUAL(-std::numeric_limits<double>::infinity(),
                          root.get("c", 0.0));
  }

  struct TestData {
    int line;
    bool ok;
    JSONCPP_STRING in;
  };
  const TestData test_data[] = {
    { __LINE__, 1, "{\"a\":9}" },         { __LINE__, 0, "{\"a\":0Infinity}" },
    { __LINE__, 0, "{\"a\":1Infinity}" }, { __LINE__, 0, "{\"a\":9Infinity}" },
    { __LINE__, 0, "{\"a\":0nfinity}" },  { __LINE__, 0, "{\"a\":1nfinity}" },
    { __LINE__, 0, "{\"a\":9nfinity}" },  { __LINE__, 0, "{\"a\":nfinity}" },
    { __LINE__, 0, "{\"a\":.nfinity}" },  { __LINE__, 0, "{\"a\":9nfinity}" },
    { __LINE__, 0, "{\"a\":-nfinity}" },  { __LINE__, 1, "{\"a\":Infinity}" },
    { __LINE__, 0, "{\"a\":.Infinity}" }, { __LINE__, 0, "{\"a\":_Infinity}" },
    { __LINE__, 0, "{\"a\":_nfinity}" },  { __LINE__, 1, "{\"a\":-Infinity}" }
  };
  for (size_t tdi = 0; tdi < sizeof(test_data) / sizeof(*test_data); ++tdi) {
    const TestData& td = test_data[tdi];
    bool ok = reader->parse(&*td.in.begin(), &*td.in.begin() + td.in.size(),
                            &root, &errs);
    JSONTEST_ASSERT(td.ok == ok) << "line:" << td.line << "\n"
                                 << "  expected: {"
                                 << "ok:" << td.ok << ", in:\'" << td.in << "\'"
                                 << "}\n"
                                 << "  actual: {"
                                 << "ok:" << ok << "}\n";
  }

  {
    char const doc[] = "{\"posInf\": Infinity, \"NegInf\": -Infinity}";
    bool ok = reader->parse(doc, doc + std::strlen(doc), &root, &errs);
    JSONTEST_ASSERT(ok);
    JSONTEST_ASSERT_STRING_EQUAL("", errs);
    JSONTEST_ASSERT_EQUAL(2u, root.size());
    JSONTEST_ASSERT_EQUAL(std::numeric_limits<double>::infinity(),
                          root["posInf"].asDouble());
    JSONTEST_ASSERT_EQUAL(-std::numeric_limits<double>::infinity(),
                          root["NegInf"].asDouble());
  }
  delete reader;
}

struct BuilderTest : JsonTest::TestCase {};

JSONTEST_FIXTURE(BuilderTest, settings) {
  {
    dgJson::Value errs;
    dgJson::CharReaderBuilder rb;
    JSONTEST_ASSERT_EQUAL(false, rb.settings_.isMember("foo"));
    JSONTEST_ASSERT_EQUAL(true, rb.validate(&errs));
    rb["foo"] = "bar";
    JSONTEST_ASSERT_EQUAL(true, rb.settings_.isMember("foo"));
    JSONTEST_ASSERT_EQUAL(false, rb.validate(&errs));
  }
  {
    dgJson::Value errs;
    dgJson::StreamWriterBuilder wb;
    JSONTEST_ASSERT_EQUAL(false, wb.settings_.isMember("foo"));
    JSONTEST_ASSERT_EQUAL(true, wb.validate(&errs));
    wb["foo"] = "bar";
    JSONTEST_ASSERT_EQUAL(true, wb.settings_.isMember("foo"));
    JSONTEST_ASSERT_EQUAL(false, wb.validate(&errs));
  }
}

struct IteratorTest : JsonTest::TestCase {};

JSONTEST_FIXTURE(IteratorTest, distance) {
  dgJson::Value json;
  json["k1"] = "a";
  json["k2"] = "b";
  int dist = 0;
  JSONCPP_STRING str;
  for (dgJson::ValueIterator it = json.begin(); it != json.end(); ++it) {
    dist = it - json.begin();
    str = it->asString().c_str();
  }
  JSONTEST_ASSERT_EQUAL(1, dist);
  JSONTEST_ASSERT_STRING_EQUAL("b", str);
}

JSONTEST_FIXTURE(IteratorTest, names) {
  dgJson::Value json;
  json["k1"] = "a";
  json["k2"] = "b";
  dgJson::ValueIterator it = json.begin();
  JSONTEST_ASSERT(it != json.end());
  JSONTEST_ASSERT_EQUAL(dgJson::Value("k1"), it.key());
  JSONTEST_ASSERT_STRING_EQUAL("k1", it.name());
  JSONTEST_ASSERT_EQUAL(-1, it.index());
  ++it;
  JSONTEST_ASSERT(it != json.end());
  JSONTEST_ASSERT_EQUAL(dgJson::Value("k2"), it.key());
  JSONTEST_ASSERT_STRING_EQUAL("k2", it.name());
  JSONTEST_ASSERT_EQUAL(-1, it.index());
  ++it;
  JSONTEST_ASSERT(it == json.end());
}

JSONTEST_FIXTURE(IteratorTest, indexes) {
  dgJson::Value json;
  json[0] = "a";
  json[1] = "b";
  dgJson::ValueIterator it = json.begin();
  JSONTEST_ASSERT(it != json.end());
  JSONTEST_ASSERT_EQUAL(dgJson::Value(dgJson::ArrayIndex(0)), it.key());
  JSONTEST_ASSERT_STRING_EQUAL("", it.name());
  JSONTEST_ASSERT_EQUAL(0, it.index());
  ++it;
  JSONTEST_ASSERT(it != json.end());
  JSONTEST_ASSERT_EQUAL(dgJson::Value(dgJson::ArrayIndex(1)), it.key());
  JSONTEST_ASSERT_STRING_EQUAL("", it.name());
  JSONTEST_ASSERT_EQUAL(1, it.index());
  ++it;
  JSONTEST_ASSERT(it == json.end());
}

JSONTEST_FIXTURE(IteratorTest, const) {
  dgJson::Value const v;
  JSONTEST_ASSERT_THROWS(
      dgJson::Value::iterator it(v.begin()) // Compile, but throw.
  );

  dgJson::Value value;

  for (int i = 9; i < 12; ++i) {
    JSONCPP_OSTRINGSTREAM out;
    out << std::setw(2) << i;
    JSONCPP_STRING str = out.str();
    value[str] = str;
  }

  JSONCPP_OSTRINGSTREAM out;
  // in old code, this will get a compile error
  dgJson::Value::const_iterator iter = value.begin();
  for (; iter != value.end(); ++iter) {
    out << *iter << ',';
  }
  JSONCPP_STRING expected = "\" 9\",\"10\",\"11\",";
  JSONTEST_ASSERT_STRING_EQUAL(expected, out.str());
}

struct RValueTest : JsonTest::TestCase {};

JSONTEST_FIXTURE(RValueTest, moveConstruction) {
#if JSON_HAS_RVALUE_REFERENCES
  dgJson::Value json;
  json["key"] = "value";
  dgJson::Value moved = std::move(json);
  JSONTEST_ASSERT(moved != json); // Possibly not nullValue; definitely not
                                  // equal.
  JSONTEST_ASSERT_EQUAL(dgJson::objectValue, moved.type());
  JSONTEST_ASSERT_EQUAL(dgJson::stringValue, moved["key"].type());
#endif
}

int main(int argc, const char* argv[]) {
  JsonTest::Runner runner;
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, checkNormalizeFloatingPointStr);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, memberCount);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, objects);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, arrays);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, arrayIssue252);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, null);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, strings);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, bools);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, integers);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, nonIntegers);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, compareNull);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, compareInt);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, compareUInt);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, compareDouble);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, compareString);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, compareBoolean);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, compareArray);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, compareObject);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, compareType);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, CopyObject);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, offsetAccessors);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, typeChecksThrowExceptions);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, StaticString);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, CommentBefore);
  // JSONTEST_REGISTER_FIXTURE(runner, ValueTest, nulls);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, zeroes);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, zeroesInKeys);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, specialFloats);
  JSONTEST_REGISTER_FIXTURE(runner, ValueTest, precision);

  JSONTEST_REGISTER_FIXTURE(runner, WriterTest, dropNullPlaceholders);
  JSONTEST_REGISTER_FIXTURE(runner, StreamWriterTest, dropNullPlaceholders);
  JSONTEST_REGISTER_FIXTURE(runner, StreamWriterTest, writeZeroes);

  JSONTEST_REGISTER_FIXTURE(runner, ReaderTest, parseWithNoErrors);
  JSONTEST_REGISTER_FIXTURE(runner, ReaderTest,
                            parseWithNoErrorsTestingOffsets);
  JSONTEST_REGISTER_FIXTURE(runner, ReaderTest, parseWithOneError);
  JSONTEST_REGISTER_FIXTURE(runner, ReaderTest, parseChineseWithOneError);
  JSONTEST_REGISTER_FIXTURE(runner, ReaderTest, parseWithDetailError);

  JSONTEST_REGISTER_FIXTURE(runner, CharReaderTest, parseWithNoErrors);
  JSONTEST_REGISTER_FIXTURE(runner, CharReaderTest,
                            parseWithNoErrorsTestingOffsets);
  JSONTEST_REGISTER_FIXTURE(runner, CharReaderTest, parseWithOneError);
  JSONTEST_REGISTER_FIXTURE(runner, CharReaderTest, parseChineseWithOneError);
  JSONTEST_REGISTER_FIXTURE(runner, CharReaderTest, parseWithDetailError);
  JSONTEST_REGISTER_FIXTURE(runner, CharReaderTest, parseWithStackLimit);

  JSONTEST_REGISTER_FIXTURE(runner, CharReaderStrictModeTest, dupKeys);

  JSONTEST_REGISTER_FIXTURE(runner, CharReaderFailIfExtraTest, issue164);
  JSONTEST_REGISTER_FIXTURE(runner, CharReaderFailIfExtraTest, issue107);
  JSONTEST_REGISTER_FIXTURE(runner, CharReaderFailIfExtraTest,
                            commentAfterObject);
  JSONTEST_REGISTER_FIXTURE(runner, CharReaderFailIfExtraTest,
                            commentAfterArray);
  JSONTEST_REGISTER_FIXTURE(runner, CharReaderFailIfExtraTest,
                            commentAfterBool);

  JSONTEST_REGISTER_FIXTURE(runner, CharReaderAllowDropNullTest, issue178);

  JSONTEST_REGISTER_FIXTURE(runner, CharReaderAllowSingleQuotesTest, issue182);

  JSONTEST_REGISTER_FIXTURE(runner, CharReaderAllowZeroesTest, issue176);

  JSONTEST_REGISTER_FIXTURE(runner, CharReaderAllowSpecialFloatsTest, issue209);

  JSONTEST_REGISTER_FIXTURE(runner, BuilderTest, settings);

  JSONTEST_REGISTER_FIXTURE(runner, IteratorTest, distance);
  JSONTEST_REGISTER_FIXTURE(runner, IteratorTest, names);
  JSONTEST_REGISTER_FIXTURE(runner, IteratorTest, indexes);
  JSONTEST_REGISTER_FIXTURE(runner, IteratorTest, const);

  JSONTEST_REGISTER_FIXTURE(runner, RValueTest, moveConstruction);

  return runner.runCommandLine(argc, argv);
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
