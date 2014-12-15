//
//  SBJEV3Brick.h
//  LEGO Control
//
//  Created by David Giovannini on 11/25/14.
//  Copyright (c) 2014 Software by Jove. All rights reserved.
//

#pragma once

#include "SBJEV3InvocationStack.h"
#include "SBJEV3DirectCommand.h"
#include "SBJEV3Connection.h"
#include "SBJEV3DeviceIdentifier.h"

#include <memory>

namespace SBJ
{
namespace EV3
{

class ConnectionFactory;
class ConnectionToken;
	
/*
 * The brick is the high-level object that represents an EV3.
 * It knows how to respond to connection events and create direct commands.
 */
 
class Brick
{
public:
	using ConnectionChanged = std::function<void(Brick& brick)>;
	using PromptBluetoothCompleted =  std::function<void(Brick& brick, bool canceled)>;

	Brick(ConnectionFactory& factory, const DeviceIdentifier& identifier = DeviceIdentifier());
	
	~Brick();
	
	ConnectionChanged connectionEvent;
	
	bool isConnected() const;
	
	void promptForBluetooth(PromptBluetoothCompleted completion);
	
	void disconnect();
		
	const DeviceIdentifier& identifier() const
	{
		return _identifier;
	}
	
	Connection::Type connectionType() const
	{
		return _connectionType;
	}
	
	ReplyStatus replyStatus() const
	{
		return _replyStatus;
	}
	
	template <typename...  Opcodes>
	typename DirectCommand<Opcodes...>::Results directCommand(float timeout, Opcodes... opcodes)
	{
		DirectCommand<Opcodes...> command(_messageCounter, timeout, opcodes...);
		_messageCounter++;
		Invocation invocation(std::move(command.invocation()));
		InvocationScope invocationScope(_stack, invocation);
		_replyStatus = command.status();
		return command.wait();
	}

private:
	DeviceIdentifier _identifier;
	InvocationStack _stack;
	Connection::Type _connectionType = Connection::Type::none;
	ReplyStatus _replyStatus = ReplyStatus::none;
	std::unique_ptr<ConnectionToken> _token;
	unsigned short _messageCounter = 0;
};
	
}
}
