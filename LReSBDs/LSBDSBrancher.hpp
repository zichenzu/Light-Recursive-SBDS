
//***********************************************************************//

#include <gecode/int/branch.hh>
#include <gecode/int.hh>
#include <gecode/LReSBSD/notq.hpp>


namespace Gecode {

	
	IntArgs t_myindex,t_myval,my_record;
    
	int r_my;
	LReSBDS_Sym *Sym;	
  

    void LReSBDS_Sym::symGoal(ViewArray<Int::IntView>& vars, int index, int val) {

        // special for IntVars but could be generalised
        if (_nSym == 0) { return ;} // i.e. no goal 	
					       // better way to return Truth?
        else { 
            int i = 0; 
			Index_class k=(*_symmetries)(_nSymR[i],index,val);
  
            for (; i < _nSym; i++) 
     
			{  	
			    if(_symvars[i].zero())      continue;
				k=(*_symmetries)(_nSymR[i],index,val); 
				
 				
                if(_symvars[i].one()&&my_record[k.index*dosize+k.val]!=-1)
				{   
 
					t_myindex[r_my]=k.index;
					t_myval[r_my++]=k.val;my_record[k.index*dosize+k.val]=-1;
					symGoal(vars,k.index,k.val);
				}
				else if(!_symvars[i].one())
				{  	 
					notq(_home,vars,_symvars[i],k.index,k.val);
				}
					 
            }
        }
    };
                                        
	LReSBDS_Sym::LReSBDS_Sym(Space& home, int nSym, 
                        int var_n,int dosize0) :
						_home(home),
						_nSym(nSym),
						_symvars(_home,nSym,0,1),
						dosize(dosize0)
	{
		for(int i=0;i<_nSym;i++)
			rel(home,_symvars[i]==1);

		_nSymR=IntArgs(_nSym); 
			
		t_myindex=IntArgs(var_n*dosize);
		t_myval=IntArgs(var_n*dosize);
		my_record=IntArgs(var_n*dosize);
		for(int i=0;i<_nSym;i++)
			_nSymR[i]=i;

		 

	};
	LReSBDS_Sym::LReSBDS_Sym(Space& home,LReSBDS_Sym OldSym) :
						_nSym(OldSym.getNSym()),
						_home(home),
						_symvars(_home,OldSym.getNSym(),0,1),
						dosize(OldSym.dosize),
						_nSymR(OldSym.getNSym())
	{
		
		for(int i=0;i<_nSym;i++)	
			_nSymR[i]=OldSym._nSymR[i];


	};
	void LReSBDS_Sym::SymAdjust(ViewArray<Int::IntView>& vars, int index, int val) 
	{
		 int oldNSym = _nSym; 
		 _nSym = 0;
		 
		 for (int i = 0 ; i < oldNSym ; i++) {
			if (!_symvars[i].zero()) 
			{
				Index_class k=(*_symmetries)(_nSymR[i],index,val);
				 
				if(k.index==index&&k.val==val)  
					_symvars[_nSym] = _symvars[i]; 
				else
				{
					//std::cout<<_nSymR[i]<<":"<<k.index<<","<<k.val<<"\n";  
					BoolVar f=_symvars[i];
					_symvars[_nSym]=BoolVar(_home,0,1);
					boolEp(_home,vars,_symvars[_nSym],f,k.index,k.val);
				 
				}
				_nSymR[_nSym++]=_nSymR[i];
			
			}
		}
	 
};

 

//**************************************************//
//**************************************************//
//**************************************************//
//**************************************************//
//**************************************************//
//*************   branch ***************************//



 template<class View, int n, class Val, unsigned int a>
class LReSBDSBrancher : public ViewValBrancher<View,n,Val,a> {
    typedef typename ViewBrancher<View,n>::BranchFilter BranchFilter;
  public:
    /// Array of symmetry implementations
    LReSBDS_Sym SymObject;
	int dosize;
	int start;
  protected:
    /// Constructor for cloning \a b
    LReSBDSBrancher(Space& home, bool share, LReSBDSBrancher& b);
    /// Constructor for creation
	
	
    LReSBDSBrancher(Home home, 
                 ViewArray<View>& x,
                 ViewSel<View>* vs[n], 
                 ValSelCommitBase<View,Val>* vsc,
                 int nSym, int dosize0,
                 BranchFilter bf,IntVarValPrint vvp);
  public:
    /// Return choice
    virtual const Choice* choice(Space& home);
    /// Return choice
    virtual const Choice* choice(const Space& home, Archive& e);
    /// Perform commit for choice \a c and alternative \a b
    virtual ExecStatus commit(Space& home, const Choice& c, unsigned int b);
    /// Perform cloning
    virtual Actor* copy(Space& home, bool share);
    /// Perform dispose
    virtual size_t dispose(Space& home);
    /// Delete brancher and return its size
    static BrancherHandle post(Home home,
                               ViewArray<View>& x,
                               ViewSel<View>* vs[n],
                               ValSelCommitBase<View,Val>* vsc,
                               int nSym, int dosize0,
                               BranchFilter bf,IntVarValPrint vvp);
  };

  template<class View, int n, class Val, unsigned int a>
  LReSBDSBrancher<View,n,Val,a>
  ::LReSBDSBrancher(Home home, ViewArray<View>& x,
                 ViewSel<View>* vs[n],
                 ValSelCommitBase<View,Val>* vsc,
                 int nSym, int dosize0,
                 BranchFilter bf,IntVarValPrint vvp)
    : ViewValBrancher<View,n,Val,a>(home, x, vs, vsc, bf,vvp),
      SymObject(home,nSym,x.size(),dosize0),
	  dosize(dosize0)
  {  	
	start=0;
	home.notice(*this, AP_DISPOSE);

	 
  }

  template<class View, int n, class Val, unsigned int a>
  forceinline BrancherHandle
  LReSBDSBrancher<View,n,Val,a>::
  post(Home home, ViewArray<View>& x,
       ViewSel<View>* vs[n], ValSelCommitBase<View,Val>* vsc,
       int nSym, int dosize0,
       BranchFilter bf,IntVarValPrint vvp) {
    return *new (home) LReSBDSBrancher<View,n,Val,a>(home,x,vs,vsc,nSym,dosize0,bf,vvp);
  }

  template<class View, int n, class Val, unsigned int a>
  forceinline
  LReSBDSBrancher<View,n,Val,a>::
  LReSBDSBrancher(Space& home, bool shared, LReSBDSBrancher<View,n,Val,a>& b)
    : ViewValBrancher<View,n,Val,a>(home,shared,b), 
      SymObject(home,b.SymObject),
      dosize(b.dosize) {
	 start=b.start;
	 SymObject._symvars.update(home,shared,b.SymObject._symvars);
		
  }
  
  template<class View, int n, class Val, unsigned int a>
  Actor*
  LReSBDSBrancher<View,n,Val,a>::copy(Space& home, bool shared) { 
    return new (home) LReSBDSBrancher<View,n,Val,a>(home,shared,*this);
  }


  // Compute choice
  template<class View, int n, class Val, unsigned int a>
  const Choice*
  LReSBDSBrancher<View,n,Val,a>::choice(Space& home) { 
   return ViewValBrancher<View,n,Val,a>::choice(home);
 
  }

 template<class View, int n, class Val, unsigned int a>
  const Choice*
  LReSBDSBrancher<View,n,Val,a>::choice(const Space& home, Archive& e) {
    return ViewValBrancher<View,n,Val,a>::choice(home,e);
  }
template<class View, int n, class Val, unsigned int a>
  size_t
  LReSBDSBrancher<View,n,Val,a>::dispose(Space& home) {
    home.ignore(*this,AP_DISPOSE);
    (void) ViewValBrancher<View,n,Val,a>::dispose(home);
    return sizeof(LReSBDSBrancher<View,n,Val,a>);
  }
  template<class View, int n, class Val, unsigned int a>
  ExecStatus
  LReSBDSBrancher<View,n,Val,a>
  ::commit(Space& home, const Choice& c, unsigned int b) {
  

	 const PosValChoice<Val>& pvc
      = static_cast<const PosValChoice<Val>&>(c);
    int pos = pvc.pos().pos;
    int val = pvc.val();
 
	r_my=0; 
	for(int i=0;i<this->x.size()*dosize;i++) 
        my_record[i]=0;
	if(b==0)
	{
		SymObject.SymAdjust(this->x,pos,val);//ajust symmetries
		Sym=&SymObject;	
		GECODE_ME_CHECK(this->x[pos].eq(home,val));
					
		for(int i=0;i<r_my;i++)
			GECODE_ME_CHECK(this->x[t_myindex[i]].nq(home,t_myval[i]));
    }
	else
	{		
		GECODE_ME_CHECK(this->x[pos].nq(home,val));
		 
		//symmetry breaking constraint
		SymObject.symGoal(this->x,pos, val); 
		for(int i=0;i<r_my;i++)
			GECODE_ME_CHECK(this->x[t_myindex[i]].nq(home,t_myval[i]));
		int r_m1=r_my;
		//assign for notq
		Sym=&SymObject;		
		
		 
	}


    return ES_OK;
  }
  
//**************************************************//
//**************************************************//
//**************************************************//
//**************************************************//
//**************************************************//
//*************   post ***************************//

  


  BrancherHandle
  branch(Home home, const IntVarArgs& x,
         IntVarBranch vars, IntValBranch vals,
         int nSym, SymmetryFunction symfunctions,int dosize0,IntBranchFilter bf=NULL) {
    using namespace Int;
    if (home.failed()) return BrancherHandle();
    ViewArray<IntView> xv(home,x);
    ViewSel<IntView>* vs[1] = { 
      Branch::viewselint(home,vars) 
    };
	_symmetries=symfunctions;
      return LReSBDSBrancher<IntView,1,int,2>::post
        (home,xv,vs,Branch::valselcommitint(home,x.size(),vals),nSym,dosize0,bf,NULL);
    }
 
}


  


