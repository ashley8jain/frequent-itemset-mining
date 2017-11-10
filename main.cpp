#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <set>
using namespace std;

int main(int argc, const char * argv[]) {
    
    string input_file;
    float sup_thre;
    int num_item=0;
    
    //checking for arguments provided
    if(argc<3){
        cout<<"Please Specify correct arguments";
        return 0;
    }
    
    input_file = argv[1];
    sup_thre = atof(argv[2]);
    list<list<int> > transctns;
    int x;
    
    //open input file and read transactions
    ifstream infile;
    infile.open(input_file);
    string str;
    
    while(getline(infile,str)){
        
        istringstream iss(str);
        list<int> trans_i;
        while ((iss>>x)){
            if((x+1)>num_item)
                num_item=x+1;
            trans_i.push_back(x);
        }
        
        transctns.push_back(trans_i);
    }
    infile.close();
    
    int num_trans = transctns.size();
    
    list<bool*> trans_bool;
    
    //converting transaction integer into transaction bool array
    for(list<int> trans_i:transctns){
        bool *trans_bool_i = new bool[num_item];
        fill(trans_bool_i, trans_bool_i + num_item, false);
//        cout<<"hereee: "<<endl;
//        for(int i=0;i<num_item;i++){
//            cout<<trans_bool_i[i]<<" ";
//        }
//        cout<<endl;
        for(int x:trans_i){
            trans_bool_i[x]=true;
        }
        trans_bool.push_back(trans_bool_i);
    }
    cout<<endl;
    
//    for(bool* as:trans_bool){
//        for(int i=0;i<num_item;i++){
//            cout<<as[i]<<" ";
//        }
//        cout<<endl;
//    }
    
    
    
    list<list<int> > global_frequents;
    

    //initial pass
    set<list<int> > candidates;

    for(int j=0;j<num_item;j++){
        list<int> candidate;
        candidate.push_back(j);
        candidates.insert(candidate);
    }
    
    while(1){
        
        list<list<int> > frequents;
        
//        for(list<int> as:candidates){
//            cout<<"candidate: ";
//            for(int ass:as){
//                cout<<ass<<" ";
//            }
//            cout<<endl;
//        }
        
//        cout<<"candidates size: "<<candidates.size()<<endl;
        int* count = new int[candidates.size()];
        fill(count, count + candidates.size(), 0);

    
//        cout<<"trans size: "<<trans_bool.size()<<endl;

        for(auto it=trans_bool.begin();it!=trans_bool.end();it++){
            bool* trans_bool_i = *it;
            int cand_i=0;
            for(list<int> candidate:candidates){
                bool present=true;
                for(int item:candidate){
                    if(!trans_bool_i[item]){
                        present = false;
                        break;
                    }
                }
                if(present){
                    count[cand_i]++;
                }
                cand_i++;
            }
        }
    
    
        int cand_i=0;
        for(list<int> candidate:candidates){
            float sup = ((float)count[cand_i]/(float)num_trans)*100.0;
            if(sup>=sup_thre){
                frequents.push_back(candidate);
                global_frequents.push_back(candidate);
//                cout<<"freq: ";
//                for(int aa:candidate){
//                    cout<<aa<<" ";
//                }
//                cout<<" --> count: "<<count[cand_i]<<endl;
            }
            cand_i++;
        }
        
        
//        for(list<int> as:frequents){
//            cout<<"freq: ";
//            for(int ass:as){
//                cout<<ass<<" ";
//            }
//            cout<<endl;
//        }
        
        
//        cout<<"here1"<<endl;
        if(frequents.size()<2){
            break;
        }
//        cout<<"here2"<<endl;

        
    //next generation candidate
        set<list<int> > next_candidates;

        if(frequents.front().size()==1){
            for(int i=0;i<frequents.size();i++){
                list<int> cand_i = *next(frequents.begin(),i);
                for(int j=i+1;j<frequents.size();j++){
                    list<int> cand_j = *next(frequents.begin(),j);
                    list<int> new_cand = cand_i;
                    new_cand.push_back(cand_j.front());
                    next_candidates.insert(new_cand);
                }
            }
        }
        else{
            
            for(int i=0;i<frequents.size();i++){
                list<int> cand_i = *next(frequents.begin(),i);
                for(int j=i+1;j<frequents.size();j++){
                    list<int> cand_j = *next(frequents.begin(),j);
                    int diff=0;
                    int diff_ele=0;
                    for(int ele:cand_j){
                        bool found = false;
                        for(int ele2:cand_i){
                            if(ele==ele2){
                                found = true;
                                break;
                            }
                        }
                        if(!found){
                            diff++;
                            if(diff>1)
                                break;
                            diff_ele=ele;
                        }
                    }
                    if(diff==1){
                        list<int> new_cand = cand_i;
                        new_cand.push_back(diff_ele);
                        new_cand.sort();
                        if(next_candidates.find(new_cand)!=next_candidates.end()){
                            continue;
                        }
                        next_candidates.insert(new_cand);
                        
                        //check if all possible subset of generated candidate is frequent
                        for(int i=new_cand.size()-1;i>=0;i--){
                            list<int> subset = new_cand;
                            auto it = new_cand.begin();
                            advance(it, i);
                            subset.remove(*it);
                            
                            bool found = false;
                            for(list<int> freq:frequents){
                                auto it2 = subset.begin();
                                bool match = true;
                                for(int aa:freq){
                                    if(aa!=*it2){
                                        match=false;
                                        break;
                                    }
                                    it2++;
                                }
                                if(match){
                                    found=true;
                                    break;
                                }
                            }
                            if(!found){
                                next_candidates.erase(new_cand);
                                break;
                            }
//                    else{
//                        cout<<"found"<<endl;
//                    }
                
                        }
                    }
                }
            }
        }
        candidates = next_candidates;
    
    }

    
    //writing all frequent item-sets into output file
    ofstream outputfile("2014CS50280.txt");
    
    for(list<int> frequent:global_frequents){
        for(int x:frequent){
            outputfile<<x<<" ";
        }
        outputfile<<endl;
    }
    outputfile.close();
    
    
    return 0;
}
