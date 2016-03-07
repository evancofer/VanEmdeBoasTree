/*
 * VanEmdeBoas.h
 *
 * Created on: Feb 23, 2016
 *
 * Copyright 2016, Evan Cofer
 *
 * This is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * See <http://www.gnu.org/licenses/>.
 */

#ifndef VANEMDEBOAS_H_
#define VANEMDEBOAS_H_
#include <cmath>
#include <vector>
#include <iostream>

class VanEmdeBoasTree {

	typedef unsigned long u_type;
	bool empty_status = true;
	u_type 	min;
	u_type 	max;
	u_type 	m;
	u_type 	unpadded_m;
	VanEmdeBoasTree* summary;
	std::vector<VanEmdeBoasTree*> children;

private:/* Internal calculations */

	inline u_type floor_root(u_type q){
		return static_cast<u_type>(exp2(std::floor(log2(static_cast<double>(q))/2)));
	}

	inline u_type ceil_root(u_type q){
		return static_cast<u_type>(exp2(std::ceil(log2(static_cast<double>(q))/2)));
	}

	inline u_type high(u_type x){
		return static_cast<u_type>(std::floor(static_cast<long double>(x)/floor_root(m)));
	}

	inline u_type low(u_type x){
		return x%floor_root(m);
	}

	inline u_type index(u_type x, u_type y){
		return ((x*floor_root(m)) + y);
	}


public:

	VanEmdeBoasTree(u_type universe):min(0),max(0),m(universe),unpadded_m(universe),summary(nullptr){
		if((universe & (universe - 1)) != 0){
			--universe;
			universe |= universe >> 1;
			universe |= universe >> 2;
			universe |= universe >> 4;
			universe |= universe >> 8;
			universe |= universe >> 16;
			universe |= universe >> 32;
			++universe;
			m = universe;
			min = universe;
		}
		if(universe > 2){
			summary = new VanEmdeBoasTree(ceil_root(universe));
			children.reserve(ceil_root(universe));
			for(u_type i = 0; i < ceil_root(universe); ++i){
				children.push_back(new VanEmdeBoasTree(floor_root(universe)));
			}
		}
	}

	~VanEmdeBoasTree(){
		if(m > 2){
		  for (auto iter = children.begin() ; iter != children.end(); ++iter){
		     delete (*iter);
		   }
		   delete summary;

		   children.clear();
		}
	}

	inline bool empty() const{
		return(empty_status);
	}
	long minimum() const {
		if(!empty()){
			return min;
		} else {
			return -1;
		}
	}
	long maximum() const {
		if(!empty()){
			return max;
		} else {
			return -1;
		}
	}

	 long successor(u_type key) {
		 if(empty()){
			 return -1;
		 } else if(m == 2){
			 if( key == 0 and max == 1){
				 return 1;
			 } else {
				 return -1;
			 }
		 } else if(!empty() and key < min){
			 return static_cast<long>(min);
		 } else {
			 long max_low = children[high(key)]->maximum();
			 if(!(children[high(key)]->empty()) && low(key) < static_cast<u_type>(max_low)){
				 long offset = children[high(key)]->successor(low(key));
				 return index(high(key), offset);
			 } else {
				 long nxt_child = summary->successor(high(key));
				 if(nxt_child == -1){
					 return -1;
				 } else {
					 long offset = children[nxt_child]->minimum();
					 return index(nxt_child, offset);
				 }
			 }
		 }
	 }

	 long predecessor(u_type key) {
		 if(empty()){
			 return -1;
		 } else if(m == 2){
			 if(key == 1 and min == 0){
				 return 0;
			 } else {
				 return -1;
			 }
		 } else if (!empty() and key > max){
			 return max;
		 } else {
			 long min_low = children[high(key)]->minimum();
			 if(!(children[high(key)]->empty()) and low(key) > static_cast<unsigned long>(min_low)){
				 long offset = children[high(key)]->predecessor(low(key));
				 return index(high(key), offset);
			 } else {
				 long prev_child = summary->predecessor(high(key));
				 if(prev_child == -1){
					 if(!empty() and key > min){
						 return min;
					 } else {
						 return -1;
					 }
				 } else {
					 long offset = children[prev_child]->maximum();
					 return index(prev_child, offset);
				 }
			 }
		 }
	 }

	 bool search(u_type key) {
		 if(empty()){
			 return false;
		 } else if(key == min || key == max){
/*			 std::cout<<"Found:"<<"("<<key<<","<<high(key)<<","<<low(key)<<")"<<children.size()<<","<<min<<","<<max<<","<<m<<"\n";*/
			 return true;
		 } else if(m == 2){
			 return false;
		 } else {
/*			 std::cout<<"recurse:"<<"("<<key<<","<<high(key)<<","<<low(key)<<")"<<children.size()<<","<<min<<","<<max<<","<<m<<"\n";*/
			 return (children[high(key)]->search(low(key)));
		 }
	 }

     void empty_insert(u_type key){
    	empty_status = false;
 		min = key;
 		max = key;
 		return;
     }

	 void insert(u_type key) {
		 if(empty()){
			 empty_insert(key);
			 return;
		 } else {
			 if( key < min ){
				 u_type swp = key;
				 key = min;
				 min = swp;
			 }
			 if( m > 2){
				 if(children[high(key)]->empty()){
					 summary->insert(high(key));
					 children[high(key)]->empty_insert(low(key));
				 } else {
					 children[high(key)]->insert(low(key));
				 }
			 }
			 if(key > max){
				 max = key;
			 }
			 if(empty()){//XXX
				 empty_status = false;
			 }
		 }
	 }

	 void remove(u_type key) {
/*			std::cout<<"Remove:"<<"("<<key<<","<<high(key)<<","<<low(key)<<")"<<children.size()<<","<<min<<","<<max<<","<<m<<"\n";*/
		 if(empty()){
			 return;
		 } else if(min == max){
			 min = 0;
			 max = 0;
			 empty_status = true;
			 return;
		 } else if(m == 2){
			 if(key == 0){
				 min = 1;
			 } else {
				 min = 0;
			 }
			 max = min;
		 } else {
			 if(key == min){
				 long first_child = summary->minimum();
/*				 std::cout<<"also:"<<index(first_child, children[first_child]->minimum())<<first_child;if(children[first_child]->empty()){std::cout<<"empt";} std::cout<<"\n";*/
				 if(children[first_child]->empty()){
					 min = max;
					 return;
				 }
				 key = index(first_child, children[first_child]->minimum());
				 min = key;
			 }
			 children[high(key)]->remove(low(key));
			 if(children[high(key)]->empty()){
				 summary->remove(high(key));
				 if(key == max) {
					 if(summary->empty()){
						 max = min;
					 } else {
						 max = index(summary->maximum(), children[summary->maximum()]->maximum());
					 }
				 }
			 } else if (key == max){
				 max = index(high(key),children[high(key)]->maximum());
			 }
		 }
	 }

	void pretty_print(int tabs) const {
		for(int i = 0; i<tabs; ++i){std::cout<<"   ";}
		std::cout<<"*Summary "<<m<<" elements"<<"("<<min<<","<<max<<")"<<"\n";
		if(m != 2){summary->pretty_print(tabs + 1); }
		if(m != 2){
			for(int i = 0; i<tabs; ++i){std::cout<<"   ";}
			std::cout<<"*Children:\n";
			for(auto iter = children.begin(); iter != children.end(); ++iter){
				(*iter)->pretty_print(tabs +1);
			}
		}
	}



};

#endif /* VANEMDEBOAS_H_ */

