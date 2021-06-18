#pragma once
#ifndef __ENGINE_ENUM_H__

BEGIN(Engine)

enum class EResourceType
{
	Static,
	NonStatic,
	End
};

END

#define __ENGINE_ENUM_H__
#endif