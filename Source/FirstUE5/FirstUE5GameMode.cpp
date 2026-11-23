#include "FirstUE5GameMode.h"

#include "FirstUE5Character.h"

AFirstUE5GameMode::AFirstUE5GameMode()
{
	DefaultPawnClass = AFirstUE5Character::StaticClass();
}
