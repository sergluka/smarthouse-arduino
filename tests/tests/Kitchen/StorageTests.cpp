#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Kitchen/Storage.h"

using namespace ::testing;

extern EEPROMClass<Storage> eeprom;
extern Storage storage;

void load_storage();

class StorageTests : public testing::Test
{
public:
    StorageTests()
    {
        storage_reset();
    }

    ~StorageTests()
    {
        Mock::VerifyAndClearExpectations(&eeprom);
    }
};

TEST_F(StorageTests, load_storage_fallback)
{
    Storage storage_mock = {0, {{0x11, 0x22, 0x33}, {0x21, 0x32, 0x43}}};

    EXPECT_CALL(eeprom, get(0, _)).WillOnce(DoAll(
        SetArgReferee<1>(storage_mock),
        ReturnRef(storage_mock)));

    load_storage();

    EXPECT_EQ(STORAGE_VERSION, storage.version);
    EXPECT_EQ(0xFF, storage.colors[SwitchingSource::External].R);
    EXPECT_EQ(0xFF, storage.colors[SwitchingSource::External].G);
    EXPECT_EQ(0xFF, storage.colors[SwitchingSource::External].B);
    EXPECT_EQ(0xFF, storage.colors[SwitchingSource::External].W);
    EXPECT_EQ(0xFF, storage.colors[SwitchingSource::Button].R);
    EXPECT_EQ(0xFF, storage.colors[SwitchingSource::Button].G);
    EXPECT_EQ(0xFF, storage.colors[SwitchingSource::Button].B);
    EXPECT_EQ(0xFF, storage.colors[SwitchingSource::Button].W);
}
