/*
 * bati-fusion - Utilitaire de fusion Open Street Map  
 * Copyright (c) 2010-2011 Jérôme Cornet
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

#include "OSMRectangle.h"
#include <algorithm>

OSMRectangle::OSMRectangle(const double x1, const double y1, const double x2, const double y2) :
   p1(std::min(x1, x2), std::min(y1, y2)),
   p2(std::max(x1, x2), std::max(y1, y2))
{
}

OSMRectangle intersection(const OSMRectangle & r1, const OSMRectangle & r2)
{
   if ( (r1.p1.getX() < r2.p2.getX()) &&
        (r2.p1.getX() < r1.p2.getX()) &&
        (r1.p2.getY() > r2.p1.getY()) &&
        (r2.p2.getY() > r1.p1.getY()) )
   {
      return OSMRectangle(std::max(r1.p1.getX(), r2.p1.getX()),
                          std::max(r1.p1.getY(), r2.p1.getY()),
                          std::min(r1.p2.getX(), r2.p2.getX()),
                          std::min(r1.p2.getY(), r2.p2.getY()));
                          
   }
   else 
   {
      return OSMRectangle(0, 0, 0, 0);
   }       
}

bool OSMRectangle::isInside(const OSMRectangle & r)
{
   return ((p1.getX() >= r.p1.getX()) &&
           (p2.getX() <= r.p2.getX()) &&
           (p1.getY() >= r.p1.getY()) &&
           (p2.getY() <= r.p2.getY()));
}

std::ostream & operator<<(std::ostream & os, const OSMRectangle & r)
{
   return os << "[ " << r.p1 << " " << r.p2 << " ]";
}
