#pragma once
#include <iostream>
#include <map>

class Game;

class Player
{
public:
	Player() : _game(nullptr), _name(""), _extraValues() {};
	~Player() {};

	static Player *initWithGame(Game *game) { Player *player = new Player(); player->_game = game; return player;}
	static Player *initWithName(const std::string &name) { Player *player = new Player(); player->_name = name; return player;}

	std::string		*name();
	void			setName(const std::string &name) { _name = name; }
	void            setPlayerNumber(int n) { _playerNumber = n; }
	int			 	playerNumber() { return _playerNumber; }
	Game			*game();
	int				index();
	bool			isCurrent();
	bool			isFriendly();
	bool			isUnfriendly();
	Player			*nextPlayer();
	Player			*previousPlayer();

	void			copyFrom(Player &player);

private:
	Game			*_game;
	std::string		_name;
	int				_playerNumber;
	std::map<std::string, std::string>		_extraValues;
};

