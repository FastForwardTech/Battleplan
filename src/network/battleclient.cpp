/****************************************************************************
**
** Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWebSockets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "battleclient.h"
#include <QtCore/QDebug>
#include <utility>

QT_USE_NAMESPACE

BattleClient::BattleClient(QObject *parent) :
	QObject(parent)
{
}

BattleClient::~BattleClient()
{
	m_webSocket.close();
}

void BattleClient::initializeState(State::GameState aState)
{
	state = aState;
	sendState();
}

void BattleClient::connectToServer(const QUrl &url)
{
	m_url = url;
	connect(&m_webSocket, &QWebSocket::connected, this, &BattleClient::onConnected);
	connect(&m_webSocket, SIGNAL(connected()), this, SIGNAL(connected()));
	connect(&m_webSocket, &QWebSocket::disconnected, this, &BattleClient::closed);
	m_webSocket.open(url);
}

void BattleClient::disconnect()
{
	m_webSocket.close();
}

void BattleClient::updateGridOffsetX(int aOffset)
{
	state.gridOffsetX = aOffset;
	sendState();
}

void BattleClient::updateGridOffsetY(int aOffset)
{
	state.gridOffsetY = aOffset;
	sendState();
}

void BattleClient::updateGridStep(qreal aStep)
{
	state.gridStep = aStep;
	sendState();
}

void BattleClient::updatePlayers(QVector<Player*> aPlayers)
{
	state.players.clear();
	for (Player* pPlayer: aPlayers)
	{
		State::Player player;
		player.name = pPlayer->getName();
		player.x = pPlayer->getGridPos().x();
		player.y = pPlayer->getGridPos().y();
		player.red = pPlayer->color().red();
		player.green = pPlayer->color().green();
		player.blue = pPlayer->color().blue();
		player.maxHp = pPlayer->getMaxHitpoints();
		player.currHp = pPlayer->getCurrentHitpoints();
		player.conditions = pPlayer->getConditions();
		state.players.append(player);
	}
	sendState();
}

void BattleClient::onConnected()
{
	connect(&m_webSocket, &QWebSocket::binaryMessageReceived, this, &BattleClient::onBinaryMessageReceived);
}

void BattleClient::onBinaryMessageReceived(QByteArray data)
{
	State::GameState state;
	state = state.deserialize(data);
	emit stateUpdateFromServer(state);
}

State::GameState BattleClient::getState() const
{
	return state;
}

void BattleClient::sendState()
{
	m_webSocket.sendBinaryMessage(state.serialize());
}
