#include <gecode/driver.hh>                                                      
#include <gecode/int.hh>                                      
#include <gecode/minimodel.hh>                                        
#include <gecode/LReSBDS/LSBDSBrancher.hpp>                                                                    
#include <gecode/LReSBDS/SBDSBrancher.hpp>                                                                    
                               
using namespace Gecode;          
 
/**  
 * \brief %Example: n-%Queens puzzle  
 *
 * Place n queens on an n times n chessboard such that they do not
 * attack each other.  
 *    
 * \ingroup Example
 *                                
 */
         
int nQueen;  
     
 Index_class symmetries (int id, int index, int val){    
   
if(id==2)  
     return Index_class(nQueen-1-val,index);  //r90

if(id==3)
     return Index_class(nQueen-1-index,nQueen -1-val) ; //r180 

if(id==4) 
    return Index_class(val,nQueen-1-index) ;  //r270   
 
if(id==0)    
    return Index_class(index,nQueen-1-val);   //rx
       
if(id==5)   
    return Index_class(nQueen-1-index,val);   //ry        
      
if(id==1)    
    return Index_class(val,index);  //d1            
   
if(id==6)   
    return Index_class(nQueen-1-val,nQueen-1-index);  //d2
  
}  
       
   
class Queens : public Script {      
protected:     
  /// Position of queens on boards   
  IntVarArray q;   
  const SizeOptions& opt;   
public:
   enum {    
         
         SBDS,
         PARSBDS,
		 LRESBDS,
		 LDSB
   };   
  /// The actual problem     
  Queens(const SizeOptions& _opt)      
    : opt(_opt), q(*this,_opt.size(),0,_opt.size()-1) {               
    const int n = q.size();  
    nQueen=n;       
     
    distinct(*this, IntArgs::create(n,0,1), q, opt.icl());
    distinct(*this, IntArgs::create(n,0,-1), q, opt.icl());
    distinct(*this, q, opt.icl());  
      
             
                       
	switch (opt.search()) {
	case SBDS:
		branch(*this,q,INT_VAR_NONE(), INT_VAL_MIN(),7,symmetries); 
		break;
	case PARSBDS:
		branch(*this,q,INT_VAR_NONE(), INT_VAL_MIN(),2,symmetries); 
	case LRESBDS:    
		branch(*this,q,INT_VAR_NONE(), INT_VAL_MIN(),2,symmetries,opt.size()); 
		break;
	case LDSB:
	{
		//LDSB Symmetry
		Symmetries s;
		IntVarArgs a1;
		IntVarArgs a2;
		for (int i = 0 ; i < n/2 ; i++) { 
			a1 << q[i];
			a2 << q[n-i-1];
		}
		IntVarArgs aboth;
		aboth << a1;
		aboth << a2;   
		s << VariableSequenceSymmetry(aboth, n/2);
		s << values_reflect(0,n-1);
		branch(*this,q,INT_VAR_NONE(), INT_VAL_MIN(),s); 
		break;
	}
	default: GECODE_NEVER;
	}
  }  
                         
                         
  /// Constructor for cloning \a s          
  Queens(bool share, Queens& s) : Script(share,s), opt(s.opt)  {                                        
    q.update(*this, share, s.q);      
  }                
                     
  /// Perform copying during cloning    
  virtual Space*               
  copy(bool share) {              
    return new Queens(share,*this);    
  }         
  /// Print solution     
  virtual void
  print(std::ostream& os) const {   
  /* 
	for (int i = 0; i < q.size(); i++) 
		os << q[i] << ", ";
    os << std::endl;
 */  
 }    
};          
 
    
   
int  
main(int argc, char* argv[]) { 
 
  SizeOptions opt("Queens");
  opt.search(Queens::SBDS);//default model
  opt.search(Queens::SBDS, "sbds", "use sbds method");
  opt.search(Queens::PARSBDS, "parsbds", "use parsbds method");
  opt.search(Queens::LRESBDS, "lresbds", "use lresbds method");
  opt.search(Queens::LDSB, "ldsb", "use ldsb method");
  
  opt.size(10);   
  opt.solutions(0);         
  opt.c_d(1);     
  opt.parse(argc,argv);      
  Script::run<Queens,DFS,SizeOptions>(opt);
  return 0;
}
 

 


