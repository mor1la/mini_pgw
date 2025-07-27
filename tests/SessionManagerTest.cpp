#include <gtest/gtest.h>
#include "../server/SessionManager.h"
#include "mocks/MockCdrWriter.h"
#include <chrono>
#include <thread>
#include <spdlog/sinks/null_sink.h>

using ::testing::_;

class SessionManagerTest : public ::testing::Test {
protected:
    SessionManagerTest() 
        : mockCdrWriter(std::make_shared<MockCdrWriter>()),
          settings{1, {"123456789012345"}} // timeoutSeconds = 1, blacklist содержит 1 элемент
    {
        auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();
        testLogger = std::make_shared<spdlog::logger>("test_logger", null_sink);

        sessionManager = std::make_unique<SessionManager>(
            settings,
            *mockCdrWriter,
            testLogger
        );
    }

    SessionManagerSettings settings;
    std::shared_ptr<MockCdrWriter> mockCdrWriter;
    std::shared_ptr<spdlog::logger> testLogger;
    std::unique_ptr<SessionManager> sessionManager;
};



TEST_F(SessionManagerTest, InitSessionCreatesNewSessionForValidImsi) {
    std::string imsi = "123456789012346";
    
    EXPECT_CALL(*mockCdrWriter, write(imsi, CdrWriter::Action::Create)).Times(1);
    
    bool result = sessionManager->initSession(imsi);
    EXPECT_TRUE(result);
    EXPECT_TRUE(sessionManager->hasSession(imsi));
}

TEST_F(SessionManagerTest, InitSessionRejectsBlacklistedImsi) {
    std::string blacklistedImsi = "123456789012345";
    
    EXPECT_CALL(*mockCdrWriter, write(blacklistedImsi, CdrWriter::Action::Reject)).Times(1);
    
    bool result = sessionManager->initSession(blacklistedImsi);
    EXPECT_FALSE(result);
    EXPECT_FALSE(sessionManager->hasSession(blacklistedImsi));
}

TEST_F(SessionManagerTest, InitSessionUpdatesExistingSession) {
    std::string imsi = "123456789012346";
    
    // Первый вызов - создание сессии
    EXPECT_CALL(*mockCdrWriter, write(imsi, CdrWriter::Action::Create)).Times(1);
    sessionManager->initSession(imsi);
    
    // Второй вызов - обновление сессии
    EXPECT_CALL(*mockCdrWriter, write(imsi, CdrWriter::Action::Update)).Times(1);
    bool result = sessionManager->initSession(imsi);
    EXPECT_TRUE(result);
    EXPECT_TRUE(sessionManager->hasSession(imsi));
}

TEST_F(SessionManagerTest, RemoveSessionDeletesSessionAndWritesCDR) {
    std::string imsi = "123456789012346";
    
    // Создаем сессию
    EXPECT_CALL(*mockCdrWriter, write(imsi, CdrWriter::Action::Create)).Times(1);
    sessionManager->initSession(imsi);
    
    // Удаляем сессию
    EXPECT_CALL(*mockCdrWriter, write(imsi, CdrWriter::Action::Delete)).Times(1);
    sessionManager->removeSession(imsi);
    
    EXPECT_FALSE(sessionManager->hasSession(imsi));
}

TEST_F(SessionManagerTest, CleanupExpiredSessionsRemovesTimedOutSessions) {
    std::string imsi1 = "123456789012346";
    std::string imsi2 = "123456789012347";
    
    // Создаем сессии
    EXPECT_CALL(*mockCdrWriter, write(imsi1, CdrWriter::Action::Create)).Times(1);
    EXPECT_CALL(*mockCdrWriter, write(imsi2, CdrWriter::Action::Create)).Times(1);
    sessionManager->initSession(imsi1);
    sessionManager->initSession(imsi2);
    
    // Ждем, пока сессии истекут
    std::this_thread::sleep_for(std::chrono::seconds(settings.timeoutSeconds + 1));
    
    // Ожидаем записи в CDR о удалении
    EXPECT_CALL(*mockCdrWriter, write(imsi1, CdrWriter::Action::Delete)).Times(1);
    EXPECT_CALL(*mockCdrWriter, write(imsi2, CdrWriter::Action::Delete)).Times(1);
    
    auto removed = sessionManager->cleanupExpiredSessions();
    
    EXPECT_EQ(removed.size(), 2);
    EXPECT_FALSE(sessionManager->hasSession(imsi1));
    EXPECT_FALSE(sessionManager->hasSession(imsi2));
}

TEST_F(SessionManagerTest, OffloadSessionRemovesSessionAndWritesOffloadCDR) {
    std::string imsi = "123456789012346";
    
    // Создаем сессию
    EXPECT_CALL(*mockCdrWriter, write(imsi, CdrWriter::Action::Create)).Times(1);
    sessionManager->initSession(imsi);
    
    // Выгружаем сессию
    EXPECT_CALL(*mockCdrWriter, write(imsi, CdrWriter::Action::Offload)).Times(1);
    sessionManager->offloadSession(imsi);
    
    EXPECT_FALSE(sessionManager->hasSession(imsi));
}

TEST_F(SessionManagerTest, GetAllSessionsReturnsActiveSessions) {
    std::string imsi1 = "123456789012346";
    std::string imsi2 = "123456789012347";
    
    // Создаем сессии
    EXPECT_CALL(*mockCdrWriter, write(imsi1, CdrWriter::Action::Create)).Times(1);
    EXPECT_CALL(*mockCdrWriter, write(imsi2, CdrWriter::Action::Create)).Times(1);
    sessionManager->initSession(imsi1);
    sessionManager->initSession(imsi2);
    
    auto sessions = sessionManager->getAllSessions();
    
    EXPECT_EQ(sessions.size(), 2);
    EXPECT_NE(sessions.find(imsi1), sessions.end());
    EXPECT_NE(sessions.find(imsi2), sessions.end());
}

TEST_F(SessionManagerTest, HasSessionReturnsFalseForNonExistentImsi) {
    std::string nonExistentImsi = "999999999999999";
    EXPECT_FALSE(sessionManager->hasSession(nonExistentImsi));
}

TEST_F(SessionManagerTest, IsBlacklistedReturnsTrueForBlacklistedImsi) {
    std::string blacklistedImsi = "123456789012345";
    EXPECT_TRUE(sessionManager->isBlacklisted(blacklistedImsi));
}

TEST_F(SessionManagerTest, IsBlacklistedReturnsFalseForNonBlacklistedImsi) {
    std::string validImsi = "123456789012346";
    EXPECT_FALSE(sessionManager->isBlacklisted(validImsi));
}