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

#include "OSMWay.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include "utils.h"

OSMWay::OSMWay() :
   id(0),
   building(false)
{
}

OSMWay::OSMWay(const std::map<int64_t, OSMNode *> & input_nodes, TiXmlElement * element) :
   id(0),
   building(false)
{
   if (element->Value() != std::string("way"))
   {
      std::cerr << "Unexpected '" << element->Value() << "' XML node, expected 'way'" << std::endl;
      throw std::runtime_error("wrong way");
   }
   
   std::string id_string;
   
   element->QueryStringAttribute("id", &id_string);
   
   std::stringstream ss;
   
   ss << id_string;
   ss >> id;
   
   element->QueryStringAttribute("timestamp", &timestamp);
   element->QueryStringAttribute("uid",       &uid);
   element->QueryStringAttribute("user",      &user);
   element->QueryStringAttribute("version",   &version);
   element->QueryStringAttribute("changeset", &changeset);
   
   TiXmlHandle refH(element);
   TiXmlElement *ref = refH.FirstChild().Element();
   
   for (; ref; ref=ref->NextSiblingElement())
   {
      if (ref->Value() == std::string("nd"))
      {
         int64_t     refID = 0;
         std::string refID_string;
   
         ref->QueryStringAttribute("ref", &refID_string);
   
         std::stringstream ss;
   
         ss << refID_string;
         ss >> refID;
                  
         std::map<int64_t, OSMNode *>::const_iterator it = input_nodes.find(refID);
         if (it != input_nodes.end())
         {
            nodes.push_back((*it).second);
         }
         else 
         {
            std::cerr << "Node id " << refID << " not found in the list of nodes (when parsing way id ";
            std::cerr << id << ")!" << std::endl;
            throw std::runtime_error("node id not found");
         }
      }
      else if (ref->Value() == std::string("tag"))
      {
         std::string key, value;
         ref->QueryStringAttribute("k", &key);
         ref->QueryStringAttribute("v", &value);      
         
         if (!key.empty() && !value.empty())
         {
            //std::cout << "Way Reading key: " << key << " value: " << value << std::endl;
            tags[key] = value;
            
            if ((key == "building") && (value != "no"))
               building = true;
         }
      }
   }
   
   /*std::cout << "way id: " << id << std::endl;
   for (std::vector<OSMNode *>::const_iterator it = nodes.begin(); it != nodes.end(); ++it)
      std::cout << "   ref id: " << (*it)->getID() << std::endl;*/
}

OSMWay::OSMWay(const OSMWay & way)
{
   id = way.id;
   building = way.building;
   
   timestamp = way.timestamp;
   uid       = way.uid;
   user      = way.user;
   version   = way.version;
   changeset = way.changeset;
   
   tags = way.tags;
   
   for (size_t i=0; i<way.nodes.size(); i++)
      nodes.push_back(new OSMNode(*way.nodes[i]));
}

OSMRectangle OSMWay::getBoundingBox() const
{
   double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
   
   for (size_t i=0; i<nodes.size(); i++)
   {
      if (i == 0)
      {
         x1 = x2 = nodes[i]->getX();
         y1 = y2 = nodes[i]->getY();
      }
      else
      {
         if (nodes[i]->getX() < x1)
            x1 = nodes[i]->getX();
         if (nodes[i]->getX() > x2)
            x2 = nodes[i]->getX();
         if (nodes[i]->getY() < y1)
            y1 = nodes[i]->getY();
         if (nodes[i]->getY() > y2)
            y2 = nodes[i]->getY();
      }
   }
   
   return OSMRectangle(x1, y1, x2, y2);
}

/*bool OSMWay::isBuilding() const
{
   std::map<std::string, std::string>::const_iterator it;
   
   for (it = tags.begin(); it != tags.end(); ++it)
   {
      if (((*it).first == std::string("building") &&
          (*it).second == std::string("yes")))
      {
         return true;
      }
   }
   
   return false;
}*/

void OSMWay::addNodePointer(OSMNode * node)
{
   nodes.push_back(node);
}

void OSMWay::dumpOSM(std::ostream & os)
{
   os << "  <way id='" << id << "' visible='true'";
   
   if (!timestamp.empty())
   {
      os << " timestamp='" << timestamp << "'";
   }
   if (!uid.empty())
   {
      os << " uid='" << uid << "'";
   }
   if (!user.empty())
   {
      os << " user='" << user << "'";
   }
   if (!version.empty())
   {
      os << " version='" << version << "'";
   }
   if (!changeset.empty())
   {
      os << " changeset='" << changeset << "'";
   }
   
   os << ">\n";
   
   for (size_t i=0; i<nodes.size(); ++i)
   {
      os << "    <nd ref='" << nodes[i]->getID() << "' />\n";
   }
   
   for (TagsConstIterator it = tags.begin(); it != tags.end(); ++it)
   {
      os << "    <tag k='" << (*it).first << "' v='" << escape_xml_char((*it).second) << "' />\n";
   }
   
   os << "  </way>\n";
}

void OSMWay::importTags(const OSMWay & original, bool avoidSource)
{
   // Import tags of way itself
   for (TagsConstIterator it = original.tags.begin(); it != original.tags.end(); ++it)
   {
      // If tags does not already exist in destination
      if (tags.find((*it).first) == tags.end())
      {
         if (avoidSource && ((*it).first == "source"))
            continue;
         
         tags[(*it).first] = (*it).second;
      }
   }
   
   // Try to import tag of nodes as well
   for (size_t i=0; i<original.nodes.size(); ++i)
   {
      const OSMNode & currentNode = *original.nodes[i];
      
      if (currentNode.getTagsBegin() != currentNode.getTagsEnd())
      {
         // Find the nearest point in the current way
         double minSquareDistance;
         size_t nodeIndex = 0;
         
         for (size_t j = 0; j < nodes.size(); ++j)
         {
            if (j == 0)
               minSquareDistance = squareDistance(currentNode, *nodes[j]);
            else if (squareDistance(currentNode, *nodes[j]) < minSquareDistance)
            {
               minSquareDistance = squareDistance(currentNode, *nodes[j]);
               nodeIndex = j;
            }
         }
         
         nodes[nodeIndex]->importTags(currentNode, avoidSource);
      }
   }
}

std::string OSMWay::getTagValueForKey(const std::string & key) const
{
   TagsConstIterator it = tags.find(key);
  
   if (it == tags.end())
   {
      return std::string();
   }
   else
   {
      return (*it).second;
   }
}

