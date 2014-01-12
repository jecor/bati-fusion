/*
 * bati-fusion - Utilitaire de fusion Open Street Map  
 * Copyright (c) 2010-2014 Jérôme Cornet
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OSM_RELATION_H
#define OSM_RELATION_H

#include <inttypes.h>
#include <vector>
#include "tinyxml.h"
#include "OSMWay.h"

class OSMRelation
{
public:
   typedef std::vector<std::pair<std::string, OSMWay *> >    MembersContainer;
   typedef MembersContainer::const_iterator   MembersConstIterator;
   typedef MembersContainer::iterator         MembersIterator;
   
   typedef std::map<std::string, std::string> TagsContainer;
   typedef TagsContainer::const_iterator      TagsConstIterator;
   typedef TagsContainer::iterator            TagsIterator;
   
   
   OSMRelation();
   
   OSMRelation(const std::map<int64_t, OSMWay *> & ways, TiXmlElement * element);
   
   int64_t getID() const           { return id;  }
   void setID(const int64_t value) { id = value; }
      
   bool isMultipolygon() const;   
   
   unsigned int getNbMembers() const { return members.size(); }
   
   const std::pair<std::string, OSMWay *> & getMember(const unsigned int index) const { return members[index]; }
   
   bool refers(const std::string & role, const OSMWay & way) const;
   
   void precopy(const OSMRelation & original) { id = original.id; tags = original.tags; }
   
   void addMemberPointer(const std::string & role, OSMWay *way); 
      
   void dumpOSM(std::ostream & os);
      
private:
   int64_t          id;
   std::string      timestamp;
   std::string      uid;
   std::string      user;
   std::string      version;
   std::string      changeset;
   MembersContainer members;
   TagsContainer    tags;
};

#endif

