#pragma once
#include "../src/server/CdrWriter.h"
#include <gmock/gmock.h>

class MockCdrWriter : public CdrWriter {
public:
    MockCdrWriter() : CdrWriter("test_cdr.log") {}
    
    MOCK_METHOD(void, write, (const std::string& imsi, Action action), (override));
};