/* -*- mode:C++ -*-
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2012, Markus Schütz
//
// This library is free software. You can redistribute  and/or  modify it under
// the terms of the GNU Lesser General Public License  as published by the Free
// Software Foundation.  Either version 2.1 of the License, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful,  but WITHOUT
// ANY  WARRANTY.  Without  even  the  implied  warranty  of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
// for more details.
//
// You should have received  a  copy  of  the GNU Lesser General Public License
// along with this library. If not, write to the Free Software Foundation, Inc,
// 59  Temple Place,  Suite 330,  Boston,  MA  02111-1307  USA,  or consult URL
// http://www.gnu.org/licenses/lgpl.html.
//
///////////////////////////////////////////////////////////////////////////////
*/
#include <gtest/gtest.h>
#include <lisle/Condition>
#include <lisle/create>
#include <lisle/self>
#include <cstdio>

using namespace lisle;

class Game
{
public:
  enum State { Start, Player1, Player2, Over, Gone1, Gone2 };
  Condition statechange;
	Mutex guard;
	Game () {}
  State state () { return data.state; }
  void state (State value) { data.state = value; }
  void lock () { guard.lock(); }
  void unlock () { guard.unlock(); }
private:
	struct Data
	{
		State state;
	} data;
};

static const wchar_t* player1 = L"Federer";
static const wchar_t* player2 = L"Agassi";
static Game game;

static void play_game ()
{
	Game::State other;

	game.lock();
	if (game.state() == Game::Player1)
		other = Game::Player2;
	else
		other = Game::Player1;

	// Play loop
	while (game.state() < Game::Over)
	{
		// Play the ball
		switch (game.state())
		{
			case Game::Start :
				printf("Game start\t");
				break;
			case Game::Player1 :
				printf("%ls\t", player1);
				break;
			case Game::Player2 :
				printf("%ls\t", player2);
				break;
			case Game::Over :
				printf("Game over\t");
				break;
			case Game::Gone1 :
				printf("%ls gone\t", player1);
				break;
			case Game::Gone2 :
				printf("%ls gone\t", player2);
				break;
		}
		
		// Now it's other player's turn
		game.state(other);
		
		// Signal the other player that now it is his turn
		game.statechange.signal();
		
		// Wait until other player finishes playing the ball
		do
		{
			game.statechange.wait(game.guard);
			if (game.state() == other)
			{
				printf("--- Player ");
				if (game.state() == Game::Player1)
					printf("%ls", player1);
				else
					printf("%ls", player2);
				printf(": SPURIOUS WAKEUP!!!\n");
			}
		}
		while (game.state() == other);
	}

	// This player gone
	game.state((Game::State)(game.state()+1));
	printf("Player gone\t");
	
	// No more access to state variable needed
	game.unlock();
	
	// Signal this player gone event
	game.statechange.broadcast();
}

static void play_tennis ()
{
	Thrid player1;
	Thrid player2;

	// Set initial state
	game.state(Game::Start);
	
	// Create players
	player1 = create(Thread(&play_game));
	player2 = create(Thread(&play_game));

	// Give them 0.001 sec to play the match
	sleep(Duration(0.001));

	// Set game over state
	game.lock();
	game.state(Game::Over);
	
	// Let them know
	game.statechange.broadcast();
	
	// Wait for players to stop
	do
	{
		game.statechange.wait(game.guard);
	}
	while (game.state() < Game::Gone2);

	// Cleanup
	printf("GAME OVER\n");
	game.unlock();
	player1.join();
	player2.join();
}

TEST (tennisTest, success)
{
	ASSERT_NO_THROW(play_tennis());
}
