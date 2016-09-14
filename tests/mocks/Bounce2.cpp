#include "Bounce2.h"

Bounce::Bounce()
{
    allow(this);

    ON_CALL(*this, attach(_)).WillByDefault(Return());
    ON_CALL(*this, attach(_, _)).WillByDefault(Return());
    ON_CALL(*this, interval(_)).WillByDefault(Return());
    ON_CALL(*this, update()).WillByDefault(Return());
    ON_CALL(*this, read()).WillByDefault(Return(false));
}
