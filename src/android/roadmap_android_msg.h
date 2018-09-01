/* roadmap_android_msg.h - Messages definitions
 *
 * LICENSE:
 *
 *   Copyright 2008 Alex Agranovich
 *
 *   This file is part of RoadMap.
 *
 *   RoadMap is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   RoadMap is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with RoadMap; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * DESCRIPTION:
 *
 *
 *
 *
 */

#ifndef INCLUDE__ROADMAP_ANDROID_MSG__H
#define INCLUDE__ROADMAP_ANDROID_MSG__H

//!!!! The category masks are two MS bytes
//!!!! Two LS bytes are assigned to the message ID
#define ANDROID_MSG_CATEGORY_IO_CALLBACK	0x010000
#define ANDROID_MSG_CATEGORY_IO_CLOSE		0x020000
#define ANDROID_MSG_CATEGORY_TIMER			0x040000
#define ANDROID_MSG_CATEGORY_MENU			0x080000
#define ANDROID_MSG_CATEGORY_RENDER			0x100000
#define ANDROID_MSG_CATEGORY_RESOLVER     0x200000

#define ANDROID_MSG_ID_MASK			0xFFFF



#endif // INCLUDE__ROADMAP_ANDROID_MSG__H

