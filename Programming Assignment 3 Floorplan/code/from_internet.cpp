/* 
 * ************************************************
 * Sourav Sanyal 
 * A02262314
 * Bridge Lab
 * Utah State University
 * MS in Electrical and Computer Engineering
 **************************************************/
#include<iostream>
#include<vector>
#include<cstring>
#include<string>
#include<map>
#include<stack>
#include<algorithm>
#include<fstream>
#include<cmath>
#include<stdlib.h>
#define H "H"
#define V "V"
#define combo
#define nmoves    10
#define ratio     0.85
#define t0        -1
#define lambdatf  0.005
#define iseed     3
#define epsilon   0.001
#define P         0.99



using namespace std;

typedef map<string,pair<float,float> > module ; // module maps name of module with width-height combination of the module//

class block{

	string name; //name of block

	vector<pair<float,float> > asp;//(width,height) of block


	friend class NPE; // NPE objects need to access private members of class block


	public:

		block(string a,vector<pair<float,float> > b):name(a),asp(b){} //block constructor

	};

class NPE{

	module m; // name and (width,height) mapping of module

 	public:

 		NPE(module x):m(x){} // NPE constructor

 		vector<block *> createNPE(vector<string>&npe); //create vector of blocks from NPE string

 		void do_stack(block *i,block *j, block *k); //do_stack is required to create slicing tree from NPE 

 		float do_cost(vector<string>& s);// calculates cost of slicing floorplan from NPE string

 		void M1(vector<string>& s);

    	void M2(vector<string>& s);

    	void M3(vector<string>& s);

    	bool skewed(vector<string>& s);

    	bool Ballot(vector<string>& s);

    	float initialTemp(vector<string>& s);

    	void SimulatedAnnealing(vector<string>& s);

    	void printNPE(vector<string>& s);

};

vector<block *> NPE::createNPE(vector<string>&npe){

	vector<block *> npei; // vector of blocks which will be returned

	vector<string>::const_iterator i = npe.begin();

	for(;i!=npe.end();i++){

		string name = *i;

		if(name!=H && name!=V){ // if NPE string element is an operand H or V

			vector<pair<float,float> >asp;

			pair<float,float>d=m[name];

			asp.push_back(d);

			float w=d.first; float h=d.second;

			if(w!=h){ // rotation of block

				pair<float,float>d1;

				d1=make_pair(h,w); // h becomes width, w becomes height

				asp.push_back(d1);

			}

			block *b=new block(name,asp);

			npei.push_back(b);

			}else{ // if NPE string element is an operator

				vector<pair<float,float> >asp;

				block *c=new block(name,asp);//block is created with name of operators and (width,height) of operators

				npei.push_back(c);

			}

		}

		return npei;

	}	


void NPE::do_stack(block *i,block *j,block *k){ // i,j are operators, k is an operand

	vector<pair<float,float> >::iterator i1;// 1st operator iterator

	vector<pair<float,float> >::iterator i2; // 2nd operator iterator

	for(i1=(i->asp).begin();i1!=(i->asp).end();i1++){

		float w1=(*i1).first; float h1=(*i1).second;

		for(i2=(j->asp).begin();i2!=(j->asp).end();i2++){

			float w2=(*i2).first; float h2=(*i2).second;

			float width=0.0; float height=0.0;

			if (k->name == H) // H operand

			{
				width=max(w1,w2); // maximum of the two widths is selected

				height=h1+h2; // height is added

			} else if (k->name == V) // V operand

				{
					width=w1+w2; // width is added

					height=max(h1,h2); // maximum of the two heights is selected

				}

			pair<float,float>aspt=make_pair(width,height); // subtree is created

			(k->asp).push_back(aspt); // subtree with operator-operator-operand is pushed back into stack and will be considered an operator for the next iteration

		}

	}

	#ifdef combo // discards redundant blocks whose area is more than area of another possible combination and the another possible combination block fits inside the redundant blocks completely

	vector<pair<float,float> >::iterator i3; // first block iterator

	vector<pair<float,float> >::iterator i4; // second block iterator

	for(i3=(k->asp).begin();i3!=(k->asp).end();i3++){

		float w3=(*i3).first; float h3=(*i3).second;

		for(i4=i3+1;i4!=(k->asp).end();i4++){


			float w4=(*i4).first; float h4=(*i4).second;

			if(w3>=w4 && h3>=h4){ // second block fits inside first block completely

				i3=(k->asp).erase(i3);  // discards first block

				i3--;

				break;

			}
			if (w4>=w3 && h4>=h3){ // first block fits inside second block completely

				i4=(k->asp).erase(i4); // discards second block

				i4--;

			}

		}

	}

	#endif

}

float NPE::do_cost(vector<string>& s){

	vector<block*>npe = createNPE(s); //vector of blocks npe is created from npe string s

	stack<block*>a; // stack a is created for stack operation

	vector<block*>::iterator i = npe.begin(); //iterator for blocks in vector npe

	for(;i!=npe.end();i++){

		block *b=*i; 

		if(b->name!=H && b->name!=V){ // if block node is an operand

			a.push(b); // pushed into stack

		}else{ // if block node is an operator

			block *s1 = a.top(); 

			a.pop(); //pops last operand from stack

			block *s2= a.top();

			a.pop(); // pops another operand from stack

			do_stack(s1,s2,b); // stack operation is done for two popped operands from stack and previous operator to create a sub-tree

			a.push(b); // sub-tree is pushed back into stack 

		}

	}
	
	block *t=a.top(); //block type pointer pointed to top of stack

	vector<pair<float,float> >::iterator j=(t->asp).begin(); //iterator for (width,height) of blocks

	float costarea=100000000.0; //costarea initialized to a high value

	for(;j!=(t->asp).end();j++){

		float area=(*j).first*(*j).second; // area is calculated for every iteration of block member

		if(area<costarea){

			costarea=area; // costarea gets updated with smaller block area

		}

	}

	return costarea;
}

//swap two operands ignoring any operator chain//
//
void NPE::M1(vector<string>& s){

	int A[100]; int i,j=0;        

	for(i=0;i<s.size();i++){ // i denotes position of operand in the vector string s//

		if(s[i]!=H && s[i]!=V){  //checks if the vector s is not an operator//

			A[j]=i;   // inserts position of operators in array A//

			j++;

		}

	}

	int r=rand()%(j-1); // randomly picks an element number in A pointing to operand position//

	iter_swap(s.begin()+A[r],s.begin()+A[r+1]); // swaps the operand and the next operand in s//

	}

//complement any operator chain
//
void NPE::M2(vector<string>& s)

{

  int A[100];

  int j=0;

  for (int i=0;i+1<s.size();i++) {  // i denotes position of operand whose next element is an operator//

    if((s[i]!=H&&s[i]!=V)&&(s[i+1]==H||s[i+1]== V)) { 

      A[j]=i+1; //inserts operator position in array A//

      j++;


    }

  }

  int r=rand()%j; // randomly picks an element number in A pointing to operator position

  int k =A[r];  //  operator positon of the element number in A assigned to k

  while(k< int(s.size())) {

    if(s[k]==H||s[k]==V) { //checks if element of vector string s is an operator

      if(s[k]==H){  

        s[k]=V;  // complements H operator to V operator

      } else{

        s[k]=H; // complements V operator to H operator

      }

      k++; // increments k to point to the next element in s as we have to complement the entire operator chain

    } else {

      break;
    }

  }

}

//swap two adjacent operator and operand
//
void NPE::M3(vector<string>& s)

{

  int A[100];

  int j=0;

   for ( int i=0; i<(s.size()-1); i++) { // i denotes position of operand and i+1 position of operator

    if((s[i]!=H && s[i]!=V)&&(s[i+1]==H||s[i+1]==V)) {

      A[j]=i; // inserts operand-operator position pair to array A where position number is the operand position

      j++;

    }

  }

 do { 

    int r=rand()%(j-1); // randomly picks an element number in A pointing to operand-operator position

    int k=A[r]; // operand-operator position of the element number in A assigned to k

    iter_swap(s.begin()+k-1,s.begin()+k); // swaps the operand and operator in vector s

    }while(!(Ballot(s) && skewed(s))); // checks if after swapping balloting property is maintained and if the npe is skewed

}      


//checks for balloting property		
//
bool NPE::Ballot(vector<string>& s){

	int a=0; int b=0;

	for(int i=0;i<s.size();i++){

		if(s[i]==H || s[i]==V){ // a denotes number of operators in vector s

			a++;

		}

		if(s[i]!=H && s[i]!=V){ // b denotes number of operands in vector s

		 b++;					

		}

		if(a>=b)return false;  // if number of operands is more operators balloting property is not maintained

	}

	 return true;

}	

//checks if npe of vector s is skewed or not
//
bool NPE::skewed(vector<string>& s){

	for(int i=1;i<s.size();i++){

		if(s[i]==H||s[i]==V){ // checks if element of vector s is an operator

			string j=s[i]; // operator value of vector element s assigned to j

			if(s[i-1]==j) // checks if the adjacent element of vector element s is the same operator or not

				return false;	// npe is not skewed if there are two adjacent operators in s having same value		

			}

		}

	return true;

}

float NPE::initialTemp(vector<string>& s){ // calculates initial temperature for simulated annealing

	int n=0; // iterator for loop

        int k=0; // iterator for counting number of uphill moves

  	float c=0.0; // sum of absolute cost differences for increasing iterations which is initially 0

  	float oc=do_cost(s); // calculates old cost of initial topology

  	float nc=0.0;

    	do {

    	float d=0.0;

     	int a=rand()%3+1; // randomly picks a number between 1 to 3

        switch(a) {

    	  case 1: {

        	M1(s); // M1 move applied if randomly chosen number is 1

        	break;

      	}

      	case 2: {

        	M2(s); // M2 move applied if randomly chosen number is 2

        	break;

      	}

      	case 3: {

        	M3(s); // M3 move if randomly chosen number is 3

        	break;

      	}

    	}

     nc=do_cost(s); // calculates new cost of new topology


     d=nc-oc; // calclates difference of old cost and new cost

    if(d>0){ // checks if the move is uphill move

     oc=nc; // new cost is assigned to old cost for next iteration

     c+=d;

     k++; // increment the number of uphill moves

    } // calculates difference of old cost and new cost and adds it sum

    n++;

  } while(n<40); // executes the loop 40 times

  float ac=c/k; // calculates average difference of old cost and new cost only for uphill move

  float t=(t0*ac)/log(P); // calculates initial temperature based on this formula, P being the initial probability of uphill moves

  return t;

}


// Simulated Annealing function
//
void NPE::SimulatedAnnealing(vector<string>& s){

	const int N=nmoves*int(m.size());// calculates total number of uphill moves

	vector <string> best=s; // best stores best optimized topology

	vector <string> temp=s; // temp stores temporary optimized topology

	float T0=initialTemp(temp); // calculates initial temperature based on temporary topology for immediate iteration

	int totm, uphill, reject; // total moves, uphill moves, rejected moves respectively

	float T=T0; // initial temperature assigned to T

	int i=0;

	do{

		totm=0; uphill=0; reject=0;

		do{

			float oc=do_cost(temp); // cost of initial topology assigned to oc

			int a=rand()%3+1; // randomly chooses a number between 1 to 3 for M move

        switch(a) {

    	  case 1: {

        	M1(temp);// M1 move applied if randomly chosen number is 1

        	break;

      	}

      	case 2: {

        	M2(temp);// M2 move applied if randomly chosen number is 2

        	break;


      	}
      	case 3: {


        	M3(temp);// M3 move applied if randomly chosen number is 3

        	break;

      	} 
		}

		totm++; // increased the number of total moves by 1

		float nc=do_cost(temp);//calculates new cost of topology after an M move

        float d=nc-oc; // calculates difference of old cost and new cost

        double r=((double)rand()/(RAND_MAX)); // randomly picks a number between 0 and 1 and assigns it to r

        if(d<0||(r<exp(-1*d/T))) { // if new cost is less than old cost or if r is less than exp(-d/T)


        if(d>0) { // if new cost is more than old cost, allows uphill move

          uphill++; // increments the number of uphill moves by 1


        }

        s=temp; // accept the new topology

        if(do_cost(s)<do_cost(best)) { // checks if cost of new topology is less than cost of best

          best=s; // new topology assigned as best

        }

      } else {

        reject++; // increments number of rejected moves by 1

      }

    } while(uphill < N && totm <= 2 * N); // checks if number of uphill moves is less than N and total number of moves less than 2*N

    T=T<lambdatf*T0?0.1*T:ratio * T; // schedules new temperature

    i++;

  } while (reject/totm <= 0.95 && T >= epsilon); // checks if rejected moves is less than 95% of total moves and T is not less than epsilon



  s=best; //accepts new optimized topology


}

//prints the vector string s//
//
//
void NPE::printNPE(vector<string>& s)

{

  vector<string>::const_iterator i=s.begin();

  for( ; i!=s.end(); i++) {

    cout << *i;

  }

  cout << "\n";

}

int main()

{

	module m;

	string fileName="";

	ifstream file;

	cout<<"Enter input file"<<endl;

	cin>>fileName; //reads input file

	file.open(fileName.c_str()); //opens input file


  if (file.is_open()) {

    while (!file.eof())

    {

      char b[100];

      file.getline(b, 100);

      
      const char* t[4] = {};

      t[0] = strtok(b, "   ,\t "); // name of a block

      if (t[0]) // checking for blank line

      {

        t[1] = strtok(0, "  ,\t  "); 

        float area = atof(t[1]); // area of a block

        t[2] = strtok(0, "   ,\t ");

        float artio = atof(t[2]); // aspect ratio of a block

        m[t[0]] = make_pair(sqrt(area * artio), sqrt(area / artio));// converts (area, aratio) to (width,height)

      }

    }

    file.close();

}
	int a;

	bool exit=false; // condition for exiting switch-case set to false 

	do{

	cout<<"Enter 1 for Topology 1 , 2 for Topology 2 , 3 for Topology 3 ,4 to exit"<<endl;

	cin>>a;

	switch(a)

	{

	case(1):

	{ 

		NPE* npeo= new NPE(m);

		vector<string>npe;

		module::const_iterator i = m.begin();

		npe.push_back(i->first);


		i++;

		npe.push_back(i->first);

		i++;

		for(;i!=m.end();i++){ // creates NPE for 3a

			npe.push_back(V);

			npe.push_back(i->first);

		}

		npe.push_back(V);

		cout<<"\n Initial Topology is "<<endl;npeo->printNPE(npe);

		cout<<"\n Initial cost is "<<npeo->do_cost(npe)<<endl;

		srand(iseed);

		npeo->SimulatedAnnealing(npe);

		cout<<"\n Optimized Topology is "<<endl;npeo->printNPE(npe);

		cout<<"\n Optimized cost is "<<npeo->do_cost(npe)<<endl;



		break;

    }



    case(2):

    {

    	NPE* npeo= new NPE(m);

		vector<string>npe;

		module::const_iterator i = m.begin();

		npe.push_back(i->first);

		i++;

		npe.push_back(i->first);

		i++;

		for(;i!=m.end();i++){ // creates NPE fro 3b


			npe.push_back(H);

			npe.push_back(i->first);

		}

		npe.push_back(H);

		cout<<"\n Initial Topology is "<<endl;npeo->printNPE(npe);

		cout<<"\n Initial cost is "<<npeo->do_cost(npe)<<endl;

		srand(iseed);

		npeo->SimulatedAnnealing(npe);

		cout<<"\n Optimized Topology is "<<endl;npeo->printNPE(npe);

		cout<<"\n Optimized cost is "<<npeo->do_cost(npe)<<endl;


		break;

    }


    case(3):

    {

    	NPE* npeo= new NPE(m);

    	static const string npe1[] = {"2","1","3","5","4","6","H","7","V","H","V","a","8","V","9","H","c","V","H","g","H","i","b","d","H","k","V","H","f","e","H","V","l","H","V","j","H","V","H"};

		vector<string> npe (npe1, npe1 + sizeof(npe1) / sizeof(npe1[0]) );// creates NPE for 3c

		cout<<"\n Initial Topology is "<<endl;npeo->printNPE(npe);

		cout<<"\n Initial cost is "<<npeo->do_cost(npe)<<endl;

		srand(iseed);

		npeo->SimulatedAnnealing(npe);


		cout<<"\n Optimized Topology is "<<endl;npeo->printNPE(npe);

		cout<<"\n Optimized cost is "<<npeo->do_cost(npe)<<endl;


		break;
    }

    case(4):

    {

    	exit=true; // exit condition set to true

    	break;

    }

}

}while(!exit);

return 0;

}
