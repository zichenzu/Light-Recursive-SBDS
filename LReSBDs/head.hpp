#ifndef _HEAD    
#define _HEAD  

	//index_class
	class Index_class{
		public: 
			int index;
			int val;
			~Index_class(){};
			Index_class(int _index,int _val){index=_index;val=_val;};
			Index_class(){};	
	  };
	  typedef Index_class (*SymmetryFunction)(int ,int,int);
	  SymmetryFunction _symmetries;
	  
#endif