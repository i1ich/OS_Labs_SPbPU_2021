#pragma once

namespace GameDefines {
	constexpr int TurnInterval = 2000;
	constexpr int HostWaitTime = 5000;
	constexpr int ClientWaitTime = HostWaitTime + 3000;
	constexpr int HandshakeWaitTime = 5000;

	constexpr int TurnsWithoutAlive = 2;
	constexpr int EndGameValue = -1;

	constexpr int HostMaxValue = 100;
	constexpr int HostMinValue = 0;

	constexpr int AliveMaxValue = 100;
	constexpr int AliveMinValue = 0;

	constexpr int DeadMaxValue = 50;
	constexpr int DeadMinValue = 0;

	constexpr int HideCondValue = 70;
	constexpr int ResurrectCondValue = 20;
}