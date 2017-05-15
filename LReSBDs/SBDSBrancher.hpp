
//***********************************************************************//

#include <gecode/int/branch.hh>
#include <gecode/int.hh>
#include <gecode/LReSBDS/head.hpp>

namespace Gecode {

 
  //symmetry class
  class SBDS_Sym  {
    private:
        Space& _home;
        int _nSym;
		IntArgs _nSymR;
	public:
		BoolVarArray _symvars;
		~SBDS_Sym() {} ;
        Space& getManager() {return _home;};
        int getNSym() {return _nSym;};
        BoolVarArray getVars() {return _symvars;};
		
        void symGoal(ViewArray<Int::IntView>& vars, int index, int val );//-----------------------------this is the normal SBDS adds conditional contrains
        SBDS_Sym(Space& home, int nSym,int var_n) ;
        void SymAdjust(  ViewArray<Int::IntView>& vars, int index, int val) ;
           
		SBDS_Sym(Space& home, SBDS_Sym OldSym) ;
  };



  void SBDS_Sym::symGoal(ViewArray<Int::IntView>& vars, int index, int val) {

		 
        if (_nSym == 0) { return ;} // i.e. no goal 	
					       // better way to return Truth?
        else { 
            for (int i=0; i < _nSym; i++) 
			{  	
				if(_symvars[i].zero())      continue;
				Index_class k=(*_symmetries)(_nSymR[i],index,val); 
				if((k.index!=index||k.val!=val)&&vars[k.index].in(k.val))
					rel(_home,(!_symvars[i]||(vars[k.index]!=k.val)));
				 
            }		 
        }
	};
                                        
  SBDS_Sym::SBDS_Sym(Space& home, int nSym, 
                        int var_n) :
                _home(home),
                _nSym(nSym),
				_symvars(_home,nSym,0,1) 
	{
		for(int i=0;i<_nSym;i++)
			rel(home,_symvars[i]==1);

		_nSymR=IntArgs(_nSym); 
		
		  
		for(int i=0;i<_nSym;i++)
			_nSymR[i]=i; 
		 

	};
  SBDS_Sym::SBDS_Sym(Space& home,SBDS_Sym OldSym) :
                _nSym(OldSym.getNSym()),
                _home(home),
				_symvars(_home,OldSym.getNSym(),0,1),
				_nSymR(OldSym.getNSym())
	{
		
		for(int i=0;i<_nSym;i++)	
			_nSymR[i]=OldSym._nSymR[i];

	};
  void SBDS_Sym::SymAdjust(ViewArray<Int::IntView>& vars, 
		       int index, int val) 
	{
		int oldNSym = _nSym; 
		_nSym = 0;
		for (int i = 0 ; i < oldNSym ; i++)
			if (!_symvars[i].zero()) 
			{
				Index_class k=(*_symmetries)(_nSymR[i],index,val);
				 
				if(k.index==index&&k.val==val)  _symvars[_nSym] = _symvars[i]; else
				{
					_symvars[_nSym]=expr(_home,_symvars[i]&&(vars[k.index]==k.val));
					
			 
				}
				_nSymR[_nSym++]=_nSymR[i];
			
			}				 
	};

 

//**************************************************//
//**************************************************//
//**************************************************//
//**************************************************//
//**************************************************//
//*************   branch ***************************//



 template<class View, int n, class Val, unsigned int a>
class SBDSBrancher : public ViewValBrancher<View,n,Val,a> {
    typedef typename ViewBrancher<View,n>::BranchFilter BranchFilter;
  public:
    /// Array of symmetry implementations
    SBDS_Sym SymObject;
  protected:
    /// Constructor for cloning \a b
    SBDSBrancher(Space& home, bool share, SBDSBrancher& b);
    /// Constructor for creation
	
	
    SBDSBrancher(Home home, 
                 ViewArray<View>& x,
                 ViewSel<View>* vs[n], 
                 ValSelCommitBase<View,Val>* vsc,
                 int nSym,
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
                               int nSym, 
                               BranchFilter bf,IntVarValPrint vvp);
  };

  template<class View, int n, class Val, unsigned int a>
  SBDSBrancher<View,n,Val,a>
  ::SBDSBrancher(Home home, ViewArray<View>& x,
                 ViewSel<View>* vs[n],
                 ValSelCommitBase<View,Val>* vsc,
                 int nSym,
                 BranchFilter bf,IntVarValPrint vvp)
    : ViewValBrancher<View,n,Val,a>(home, x, vs, vsc, bf,vvp),
      SymObject(home,nSym,x.size())
  {  
     home.notice(*this, AP_DISPOSE);   
  }

  template<class View, int n, class Val, unsigned int a>
  forceinline BrancherHandle
  SBDSBrancher<View,n,Val,a>::
  post(Home home, ViewArray<View>& x,
       ViewSel<View>* vs[n], ValSelCommitBase<View,Val>* vsc,
       int nSym, 
       BranchFilter bf,IntVarValPrint vvp) {
    return *new (home) SBDSBrancher<View,n,Val,a>(home,x,vs,vsc,nSym,bf,vvp);
  }

  template<class View, int n, class Val, unsigned int a>
  forceinline
  SBDSBrancher<View,n,Val,a>::
  SBDSBrancher(Space& home, bool shared, SBDSBrancher<View,n,Val,a>& b)
    : ViewValBrancher<View,n,Val,a>(home,shared,b), 
      SymObject(home,b.SymObject){

	 SymObject._symvars.update(home,shared,b.SymObject._symvars);

  }
  
  template<class View, int n, class Val, unsigned int a>
  Actor*
  SBDSBrancher<View,n,Val,a>::copy(Space& home, bool shared) {
    return new (home) SBDSBrancher<View,n,Val,a>(home,shared,*this);
  }


  // Compute choice
  template<class View, int n, class Val, unsigned int a>
  const Choice*
  SBDSBrancher<View,n,Val,a>::choice(Space& home) { 
	return ViewValBrancher<View,n,Val,a>::choice(home);
  }

  template<class View, int n, class Val, unsigned int a>
  const Choice*
  SBDSBrancher<View,n,Val,a>::choice(const Space& home, Archive& e) {
    return ViewValBrancher<View,n,Val,a>::choice(home,e);
  }
  template<class View, int n, class Val, unsigned int a>
  size_t
  SBDSBrancher<View,n,Val,a>::dispose(Space& home) {
    home.ignore(*this,AP_DISPOSE);
    (void) ViewValBrancher<View,n,Val,a>::dispose(home);
    return sizeof(SBDSBrancher<View,n,Val,a>);
  }
  template<class View, int n, class Val, unsigned int a>
  ExecStatus
  SBDSBrancher<View,n,Val,a>
  ::commit(Space& home, const Choice& c, unsigned int b) {
  

		const PosValChoice<Val>& pvc
		  = static_cast<const PosValChoice<Val>&>(c);
		int pos = pvc.pos().pos;
		int val = pvc.val();
		
		if(b==0)
		{
			SymObject.SymAdjust(this->x,pos,val);//ajust symmetries
			ExecStatus fromBase = ViewValBrancher<View,n,Val,a>::commit(home, c, b);
			GECODE_ES_CHECK(fromBase);
		}
		else
		{
			//post symmetry breaking constraint
			SymObject.symGoal(this->x,pos, val); 
			ExecStatus fromBase = ViewValBrancher<View,n,Val,a>::commit(home, c, b);
			GECODE_ES_CHECK(fromBase);
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
         int nSym, SymmetryFunction symfunctions,IntBranchFilter bf=NULL) {
		using namespace Int;
		if (home.failed()) return BrancherHandle();
		ViewArray<IntView> xv(home,x);
		ViewSel<IntView>* vs[1] = { 
		  Branch::viewselint(home,vars) 
		};
		_symmetries=symfunctions;
		return SBDSBrancher<IntView,1,int,2>::post
		(home,xv,vs,Branch::valselcommitint(home,x.size(),vals),nSym,bf,NULL);
	}
 
}


  


