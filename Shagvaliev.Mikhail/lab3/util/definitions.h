//
// Created by Mikhail.Shagvaliev on 12/26/21.
//

#ifndef LAB3_DEFINITIONS_H
#define LAB3_DEFINITIONS_H

#include <iostream>
#include <memory>
#include <any>
#include <map>
#include <climits>
#include <functional>

typedef std::any Any;

class Set;

typedef std::shared_ptr<Set> pSet;
typedef std::function<int(std::any&)> OrderedHasher;

#endif //LAB3_DEFINITIONS_H
