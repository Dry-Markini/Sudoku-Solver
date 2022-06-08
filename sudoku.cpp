#include <iostream>
#include <cstdio>

using namespace std;

class sudoku {
  private:
    class cell {
      private:
        bool nums[9];
        int size, sum;

      public:
        // constructors
        cell() : size(9), sum(45) {
          for (int i=0; i<9; ++i) nums[i]=true;
        }
        cell(int a[], int length) : size(length), sum(0) {
          for (int i=0; i<9; ++i) nums[i]=false;
          for (int i=0; i<length; ++i) { nums[a[i]-1]=true; sum+=a[i]; }
        }
        cell(const cell& c): size(c.size), sum(c.sum) {
          for (int i=0; i<9; ++i) nums[i]=c.nums[i];
        }
        cell& operator=(const cell& c) {
          size=c.size; sum=c.sum;
          for (int i=0; i<9; ++i) nums[i]=c.nums[i];
          return *this;
        }
        // functions
        void print() { for (int i=0; i<9; ++i) if (nums[i]) cout << i+1 << " "; } // for debugging
        bool search(unsigned a) { return nums[a-1]; }
        bool remove(unsigned a) {
          if (nums[a-1]) {
            nums[a-1]=false;
            --size; sum-=a;
            if (!size) throw 1;
            return true; }
          return false;
        }
        bool determined() { return size==1; }
        int getsize() { return size; }
        int getnum() { return sum; }
        void give_array(int* array) const {
          int count=0;
          for (int i=0; i<9; ++i) if (nums[i]) array[count++]=i+1;
        }
        void set(int x) {
          for (int i=0; i<9; ++i) nums[i]=false;
          nums[x-1]=true; size=1; sum=x;
        }
    };
    class thebool {
      private:
        bool thebools[9];
      public:
        thebool() { for (int i=0; i<9; ++i) thebools[i]=false; }
        thebool(thebool &c) { for (int i=0; i<9; ++i) thebools[i]=c.thebools[i]; }
        thebool operator + (thebool& a) {
          thebool out;
          for (int i=0; i<9; ++i) out.thebools[i]=thebools[i]|a.thebools[i];
          return out;
        }
        int size() {
          int count=0;
          for (int i=0; i<9; ++i) if (thebools[i]) ++count;
          return count;
        }
        void add(int i) { thebools[i-1]=true; }
        void give_array(int* array) const {
          int count=0;
          for (int i=0; i<9; ++i) if (thebools[i]) array[count++]=i+1;
        }
    };

    cell array[81];

    static int loc(int x, int y) { return (x-1)*9+y-1; }
    static int loc(int group, int id, int mode) {
      if (mode==1) return (group-1)*9+id-1;
      if (mode==2) return (id-1)*9+group-1;
      return (group-1)/3*27 + (group-1)%3*3 + (id-1)/3*9 + (id-1)%3;
    } //mode=1,2,3 correspond to row, column and block
    static int get_block(int x, int y) { return (x-1)/3*3 + (y-1)/3 + 1; }
    static int get_block_id(int x, int y) { return  (x-1)%3*3 + (y-1)%3 + 1; }
    static int get_x(int block, int block_id) { return (block-1)/3*3 + (block_id-1)/3 + 1; }
    static int get_y(int block, int block_id) { return (block-1)%3*3 + (block_id-1)%3 + 1; }
    static void next_comb(int* comb, int size, int max) {
      int i;
      for (i=1; i<=size && comb[size-i]==max-i+1; ++i);
      if (i==size+1) { comb[0]=0; return; }
      ++comb[size-i];
      for (i=size-i+1; i<size; ++i) comb[i]=comb[i-1]+1;
    }

    void print_line (int line) {
      cout << "||";
      for (int i=0; i<3; ++i) {
        for (int j=1; j<4; ++j) {
          cell &the_cell=array[loc(line,i*3+j)];
          if (the_cell.determined()) cout << " " << the_cell.getnum() << " |";
          else cout << "   |";
        }
        cout << "|";
      }
      cout << endl;
    }
    static void line_change (char c) {
      cout << "||";
      for (int i=0; i<3; ++i) {
        for (int j=0; j<11; ++j) cout << c;
        cout << "||";
      }
      cout << endl;
    }

  public:
    //constructors and basic operations
    sudoku() {}
    sudoku(const sudoku &s) { for (int i=0; i<81; ++i) array[i]=s.array[i]; }
    sudoku(char* file_location) {
      FILE *thefile=fopen(file_location, "r");

      for (int i=1; i<10; ++i)
        for (int j=1; j<10; ++j) {
          int temp;
          fscanf(thefile, "%d", &temp);
          put(i,j,temp);
        }
      fclose(thefile);
    }

    sudoku& operator=(const sudoku &s) {
      for (int i=0; i<81; ++i) array[i]=s.array[i];
      return *this;
    }

    void put(int x, int y, int value, int exception=0) {
      if (value) { array[loc(x,y)].set(value); clear_core(x,y,get_block(x,y),get_block_id(x,y),exception); }
    }
    bool determined(int x, int y) { return array[loc(x,y)].determined(); }
    int get(int x, int y) { return array[loc(x,y)].getnum(); }

    void print() {
      line_change ('=');
      for (int i=0; i<3; ++i) {
        for (int j=1; j<4; ++j) {
          print_line(i*3+j);
          if (j!=3) line_change('-');
        }
        line_change('=');
      }
      cout << endl;
    }
    bool finished() {
      for (int i=0; i<81; ++i) if (!array[i].determined()) return false;
      return true;
    }
    void showme(int x, int y) { array[loc(x,y)].print(); } // for debugging

  private:
    //clear -- clear columr -- clear row -- clear block
    //group can be row, column or block depending on the value of "mode"
    bool clear_group(int group, int x, int mode, int spare1, int spare2=0, int spare3=0) {
      bool flag=false;
      for (int i=1; i<10; ++i) {
        if (i==spare1||i==spare2||i==spare3) continue;
        cell &temp=array[loc(group,i,mode)];
        bool temp_bool=temp.remove(x);
        if (temp_bool) {
          flag=true;
          if (temp.determined()) clear(group,i,mode); //we get a new number, so we start again
        }
      }
      return flag;
    }
    bool clear_block(int group, int id, int s, int mode) {
      if (mode==1) {
        int block=get_block(group,id);
        int temp=(group-1)%3*3+1;
        return clear_group(block,s,3,temp,temp+1,temp+2);
      }
      int block=get_block(id,group);
      int temp=(group-1)%3+1;
      return clear_group(block,s,3,temp,temp+3,temp+6);
    } // mode=1 for row and 2 for col
    bool clear(int group, int id, int mode, int exception=0) {
      if (mode==1) return clear_core(group,id,get_block(group,id),get_block_id(group,id),exception);
      if (mode==2) return clear_core(id,group,get_block(id,group),get_block_id(id,group),exception);
      return clear_core(get_x(group,id),get_y(group,id),group,id,exception);
    }
    //because group/id/mode coordinates are tough to handle we convert them to
    //x/y and pass them to clear_core() which does the actual work
    bool clear_core(int x, int y, int block, int block_id, int exception) {
      int number=array[loc(x,y)].getnum();
      bool flag=false;
      if (exception!=1 && clear_group(x,number,1,y)) flag=true;
      if (exception!=2 && clear_group(y,number,2,x)) flag=true;
      if (exception!=3 && clear_group(block,number,3,block_id)) flag=true;
      return flag;
    }
    //if exception=1,2,3 correspond to row,column and block
  public:
    //basic method
    bool set_in_group_once(int group, bool mode) {
      int freq[9]={}; //initialize everything to 0
      bool flag=false;
      for (int i=1; i<10; ++i) {
        cell &temp=array[loc(group,i,mode)];
        if (!temp.determined())
          for (int l=1;l<10;++l)
            if (temp.search(l)) ++freq[l-1];
      }
      for (int x=1; x<10; ++x)
        if (freq[x-1]==1) {
          flag=true;
          for (int i=1; i<10; ++i) {
            if (array[loc(group,i,mode)].search(x)) put(group,i,x,mode);
          }
        }
      return flag;
    }
    bool set_in_group(int group, int mode) {
      if (!set_in_group_once(group,mode)) return false;
      while (set_in_group_once(group,mode));
      return true;
    }
    bool basic_move(int mode) {
      bool flag=false;
      for (int i=1; i<10; ++i)
          if (set_in_group(i,mode)) flag=true;
      return flag;
    }

    bool basic_move() {
      bool one, two, three;
      one=basic_move(1);
      two=basic_move(2);
      three=basic_move(3);
      return one||two||three;
    }

    //second method -- clear column/row base using two or three cells
    bool superposition_ruleout_block(int x, int y) {
      bool flag=false;
      int X=(x-1)*3, Y=(y-1)*3;
      for (int i=1; i<10; ++i) {
        int row=0, col=0;
        bool found=true;
        for (int k=1; found && k<4; ++k)
          for (int l=1; found && l<4; ++l) {
            cell &temp=array[loc(X+k,Y+l)];
            if (temp.search(i) && !temp.determined()) {
              if (row==0 && col==0) { row=k; col=l; }
              else if (row!=0 && col!=0) {
                if (row==k) col=0;
                else if (col==l) row=0;
                else found=false;
              }
              else if ((row==0 && col!=l) || (col==0 && row!=k)) found=false;
            }
          }
        if ((row==0 && col==0)||(row!=0 && col!=0)) found=false;
        if (found) {
          if (row!=0) { if (clear_group(X+row,i,1,Y+1,Y+2,Y+3)) flag=true; }
          else if (clear_group(Y+col,i,2,X+1,X+2,X+3)) flag=true;
        }
      }
      return flag;
    }
    //remnant of the old methodology where x,y are 1,2,3 and denote the coords of the block
    bool second_move_block() {
      bool flag=false;
      for (int x=1; x<4; ++x)
        for (int y=1; y<4; ++y) {
          if (superposition_ruleout_block(x,y)) flag=true;
        }
      return flag;
    }

    //rc stands for row/column
    bool superposition_ruleout_rc(int rc, int mode) {
      bool flag=false;
      for (int i=1; i<10; ++i) {
        int cell_num=0, id;
        bool found=true;
        for (int k=1; found && k<10; ++k) {
          cell& temp=array[loc(rc,k,mode)];
          if (temp.search(i) && !temp.determined()) {
            if (!cell_num) { cell_num=(k-1)/3+1; id=k; }
            else if ((k-1)/3+1!=cell_num) found=false;
          }
        }
        if (!cell_num||!found) continue;

        if (clear_block(rc,id,i,mode)) flag=true;
      }
      return flag;
    }
    bool second_move_rc(int mode) {
      bool flag=false;
      for (int x=1; x<10; ++x)
          if (superposition_ruleout_rc(x,mode)) flag=true;
      return flag;
    }

    bool second_move() {
      bool one,two,three;
      one=second_move_rc(1);
      two=second_move_rc(2);
      three=second_move_block();
      return one||two||three;
    }

  private:
    bool purge_cells(int location, const thebool& target, int number_list[], int exceptions_index[], int size, int mode) {
        int targets[size];
        target.give_array(targets);
        bool flag=false;
        for (int i=0; i<size; ++i) {
          cell &temp=array[loc(location,targets[i],mode)];
          int count=0;
          for (int j=1; j<10; ++j)
            if (count>=size || j!=number_list[exceptions_index[count]-1]) {
              if (temp.remove(j)) {
                flag=true;
                if (temp.determined()) clear(location,targets[i],mode);
              }
            }
            else ++count;
        }
        return flag;
      }

  public:
	  //third method -- When two numbers occupy two cells no other
	  //number can occupy the same cells
    bool exclusive_use(int group, int howmany, int mode) {
      //measure the frequency of each number 1-9 in that group

      int freq[9]={}; //initialize everything to 0
      for (int i=1; i<10; ++i) {
        cell &temp=array[loc(group,i,mode)];
        if (!temp.determined())
          for (int l=1;l<10;++l)
            if (temp.search(l)) ++freq[l-1];
      }

      int count_nums=0, thenums[9];
      for (int i=1; i<10; ++i) if (freq[i-1] && freq[i-1]<=howmany) thenums[count_nums++]=i;

      if (count_nums<howmany) return false;

      thebool whereabouts[count_nums];
      for (int i=1; i<10; ++i) {
        cell &temp=array[loc(group,i,mode)];
        if (!temp.determined())
          for (int l=0; l<count_nums; ++l)
            if (temp.search(thenums[l])) whereabouts[l].add(i);
      }

      int groups[howmany];
      bool flag=false;
      for (int i=0; i<howmany; ++i) groups[i]=i+1;

      while (groups[0]) /*when we are done next_comb makes that 0*/ {
        thebool sum;
        for (int i=0; i<howmany; ++i) { sum=sum+whereabouts[groups[i]-1]; }
        if (sum.size()==howmany) {
          if (purge_cells(group,sum,thenums,groups,howmany,mode)) return true;
        }
        //create next group of "howmany"
        next_comb(groups, howmany, count_nums);
      }
      return false;
  	} //mode=1 for row, 2 for column and 3 for block
    bool third_move() {
      for (int howmany=2; howmany<8; ++howmany)
        for (int mode=1; mode<4; ++mode)
          for (int group=1; group<10; ++group)
            if (exclusive_use(group,howmany,mode)) return true;
      return false;
    }

  private:
    //returns 0 if solvable, 1 if unsolvable, 2 if multiple solutions exist
    //and 3 if maximum number of sudokus is reached
    int solve_core(int &guess_num) {
      while(true) {

        while (basic_move());
        if (finished()) return 0;

        if (second_move()) continue;

        if (third_move()) continue;

        // at that point we will pick a cell with the minimum amount of possible
        // nums and try all of them

        int temp, min=10, index, x, y;

        for (int i=0; i<81; ++i) {
          temp=array[i].getsize();
          if (temp==1) continue;
          if (temp<min) {
            min=temp;
            index=i;
            if (temp==2) break;
          }
        }
        x=index/9+1; y=index%9+1;

        int thenums[min];
        array[index].give_array(thenums);

        //begin the guessing procedure

        bool solvable=false;
        sudoku chosen_one;

        for (int i=0; i<min; ++i) {
          if (++guess_num>20) return 3;
          sudoku a=*this; //make a new sudoku that we will experiment on
          try {
            a.put(x,y,thenums[i]);
            int ret_save=a.solve_core(guess_num);
            if (ret_save==2||ret_save==3) return ret_save;
            if (ret_save==0) {
              if (solvable) return 2;
              solvable=true;
              chosen_one=a;
            }
          }
          catch (int exception) {}
          --guess_num;
        }
        if (!solvable) return 1;
        //at this point we have had a breakthrough so the chosen_one becomes
        //our sudoku
        *this=chosen_one;
        return 0;
      }
    }
  public:
    bool solve() {
      int count=0, temp;
      try { temp=solve_core(count); }
      catch (int exception) { temp=1;
      }
      if (temp==0) return true;
      return false;
    }
    //solvable does the same as solve() without changing the sudoku
    bool solvable() { sudoku a=*this; return a.solve(); }
};

// NOTES:
// 1) Make types more elegant (e.g. unsigned instead of int) && REFERENCES
// 2) superposition_ruleout_block check for col!=0 && row!=0 really necessary?
// 4) exclusive_use should check if it can make changes
// 5) Include the clearing method inside "cell:remove()" (elegant and safe)
// 8) In solve() when second_move() makes progress don't call the whole
//    basic_move().
// 9) In new constructor check if location is correct and the like

int main () {
  char file[] ="takefromhere.txt";

  sudoku the_sudoku(file);
  cout << "START: " << endl;
  the_sudoku.print();

  cout << the_sudoku.solve() << endl;
  the_sudoku.print();
  getchar();
}
