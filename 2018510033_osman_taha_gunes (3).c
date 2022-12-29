#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define MAX_PATIENT 1000

// The number of registration desks that are available.
int REGISTRATION_SIZE = 10;
// The number of restrooms that are available.
int RESTROOM_SIZE = 10;
//The number of cashiers in cafe that are available.
int CAFE_NUMBER = 10;
// The number of General Practitioner that are available.
int GP_NUMBER = 10;
//The number of cashiers in pharmacy that are available.
int PHARMACY_NUMBER = 10;
//The number of assistants in blood lab that are available.
int BLOOD_LAB_NUMBER = 10;
//The number of operating rooms, surgeons and nurses that are available.
int OR_NUMBER = 10;
int SURGEON_NUMBER = 30;
int NURSE_NUMBER = 30;

int copysurgeon=30;
int copynurse=30;
//The maximum number of surgeons and nurses that can do a surgery. A random value iscalculated for each operation between 1 and given values to determine the requirednumber of surgeons and nurses.
int SURGEON_LIMIT = 5;
int NURSE_LIMIT = 5;
//The number of patients that will be generated over the course of this program.
int PATIENT_NUMBER = 1000;
//The account of hospital where the money acquired from patients are stored.
int HOSPITAL_WALLET = 0;
/*The time required for each operation in hospital. They are given in milliseconds. 
Butyou must use a randomly generated value between 1 and given values below to 
determinethe time that will be required for that operation individually. 
This will increase therandomness of your simulation.
 The WAIT_TIME is the limit for randomly selected time between 1 and the given valuethat determines 
how long a patient will wait before each operation to retry to execute.Assuming 
the given department is full*/
int WAIT_TIME = 100;
int REGISTRATION_TIME = 100;
int GP_TIME = 200;
int PHARMACY_TIME = 100;
int BLOOD_LAB_TIME = 200;
int SURGERY_TIME = 500;
int CAFE_TIME = 100;
int RESTROOM_TIME = 100;
/* The money that will be charged to the patients for given operations. 
The registrationand blood lab costs should be static (not randomly decided) 
but pharmacy and cafe costshould be randomly generated between 1 and given 
value below to account for differentmedicine and food that can be purchased.
The surgery cost should calculated based on number of doctors and nurses 
that wasrequired to perform it. The formula used for this should be: 
 SURGERY_OR_COST + (number of surgeons * SURGERY_SURGEON_COST) +(number of nurses * SURGERY_NURSE_COST)*/
int REGISTRATION_COST = 100;
int PHARMACY_COST = 200; 
// Calculated randomly between 1 and given value.
int BLOOD_LAB_COST = 200;
int SURGERY_OR_COST = 200;
int SURGERY_SURGEON_COST = 100;
int SURGERY_NURSE_COST = 50;
int CAFE_COST = 200; // Calculated randomly between 1 and given value.
/* The global increase rate of hunger and restroom needs of patients. 
It will increaserandomly between 1 and given rate below.*/
int HUNGER_INCREASE_RATE = 10;
int RESTROOM_INCREASE_RATE = 10;


int numPatient=1000;//patient number setting


typedef struct Req{//requirement stuct
    int Hunger_Meter;
    int Restroom_Meter;
    int countCafe;
    int countRestroom;
}Requirement;

void *patient(void *num);//definition for use function on all code
void randwait(int secs);//
void goPharmacy(int num,Requirement req);
void goBloodlab(int num,Requirement req);
void goCafe(int num,Requirement req);
void goRestroom(int num,Requirement req);
void goOperation(int num,Requirement req);

sem_t regDesk;//created semaphores
sem_t gpRoom;
sem_t pharmacy;
sem_t bloodLab;
sem_t opRoom;
sem_t cafe;
sem_t restroom;




int main(){
    srand(time(NULL));
    
    pthread_t ptid[MAX_PATIENT];//patient threads set

    int i,x;//
    int patients[MAX_PATIENT];
    for(i=0;i< MAX_PATIENT;i++){
        patients[i]=i;//each patient take ID
    }

    sem_init(&regDesk,0,REGISTRATION_SIZE);//setting semaphore capacity

    sem_init(&gpRoom,0,GP_NUMBER);

    sem_init(&pharmacy,0,PHARMACY_NUMBER);

    sem_init(&bloodLab,0,BLOOD_LAB_NUMBER);

    sem_init(&cafe,0,CAFE_NUMBER);

    sem_init(&restroom,0,RESTROOM_SIZE);

    sem_init(&opRoom,0,OR_NUMBER);
    
    for(i=0;i<numPatient;i++){
        pthread_create(&ptid[i],NULL,patient,(void *)&patients[i]);
    }
    for(i=0;i<numPatient;i++){
        pthread_join(ptid[i],NULL);
    }
}


void *patient(void *number){
    int num = *(int *) number;
    printf("Patient %d created.\n",num);
    randwait(100);
    int pDisease = rand()%2;
    Requirement req;
    int a = rand()%100;
    int b = rand()%100;
    req.Hunger_Meter=a;//hungry and restroom requirement definition
    req.Restroom_Meter=b;
    req.countCafe=0;
    req.countRestroom=0;
    sem_wait(&regDesk);
    printf("Patient %d entering to the hospital.\n",num);
    randwait(REGISTRATION_TIME);
    ////////////////////////each between "/" checks requirement status
    req.Hunger_Meter+=rand()%10;
    req.Restroom_Meter+=rand()%10;
    if(req.Hunger_Meter>=100&&req.countCafe==0){
        goCafe(num,req);
        req.countCafe+=1;
        req.Hunger_Meter=0;        
    }   
    if(req.Restroom_Meter>=100&&req.countRestroom==0){
        goRestroom(num,req);
        req.Restroom_Meter=0;
        req.countRestroom+=1;
    }
    ////////////////////////
    sem_post(&regDesk);
    printf("Patient %d paid %d for register.\n",num,REGISTRATION_COST);
    HOSPITAL_WALLET+=REGISTRATION_COST;
    printf("Patient %d registered.\n",num);
    sem_wait(&gpRoom);
    printf("Patient %d starting to wait GP.\n",num);
    randwait(GP_TIME);
    ////////////////////////
    req.Hunger_Meter+=rand()%10;
    req.Restroom_Meter+=rand()%10;
    if(req.Hunger_Meter>=100&&req.countCafe==0){
        goCafe(num,req);
        req.countCafe+=1;
        req.Hunger_Meter=0;        
    }   
    if(req.Restroom_Meter>=100&&req.countRestroom==0){
        goRestroom(num,req);
        req.Restroom_Meter=0;
        req.countRestroom+=1;
    }
    ////////////////////////
    sem_post(&gpRoom);
    printf("Patient %d entered to GP room.\n",num);    
    
    if(pDisease==0){//pharmacy
        int r = rand()%2;
        if(r==0){//no need bloodlab
            goPharmacy(num,req);
            ////////////////////////
    req.Hunger_Meter+=rand()%10;
    req.Restroom_Meter+=rand()%10;
    if(req.Hunger_Meter>=100&&req.countCafe==0){
        goCafe(num,req);
        req.countCafe+=1;
        req.Hunger_Meter=0;        
    }   
    if(req.Restroom_Meter>=100&&req.countRestroom==0){
        goRestroom(num,req);
        req.Restroom_Meter=0;
        req.countRestroom+=1;
    }
    ////////////////////////
            
        }
        else{//need bloodlab
            goBloodlab(num,req);
            ////////////////////////
    req.Hunger_Meter+=rand()%10;
    req.Restroom_Meter+=rand()%10;
    if(req.Hunger_Meter>=100&&req.countCafe==0){
        goCafe(num,req);
        req.countCafe+=1;
        req.Hunger_Meter=0;        
    }   
    if(req.Restroom_Meter>=100&&req.countRestroom==0){
        goRestroom(num,req);
        req.Restroom_Meter=0;
        req.countRestroom+=1;
    }
    ////////////////////////
            goPharmacy(num,req);
            ////////////////////////
    req.Hunger_Meter+=rand()%10;
    req.Restroom_Meter+=rand()%10;
    if(req.Hunger_Meter>=100&&req.countCafe==0){
        goCafe(num,req);
        req.countCafe+=1;
        req.Hunger_Meter=0;        
    }   
    if(req.Restroom_Meter>=100&&req.countRestroom==0){
        goRestroom(num,req);
        req.Restroom_Meter=0;
        req.countRestroom+=1;
    }
    ////////////////////////
        }        
    }
    else //(pDisease==1) surgery
    {
        goOperation(num,req);
        ////////////////////////
    req.Hunger_Meter+=rand()%10;
    req.Restroom_Meter+=rand()%10;
    if(req.Hunger_Meter>=100&&req.countCafe==0){
        goCafe(num,req);
        req.countCafe+=1;
        req.Hunger_Meter=0;        
    }   
    if(req.Restroom_Meter>=100&&req.countRestroom==0){
        goRestroom(num,req);
        req.Restroom_Meter=0;
        req.countRestroom+=1;
    }
    ////////////////////////
        int m = rand()%2;
        if(m==0)//need medicine
        {
            //printf("pharmacy check after operation\n");
            goPharmacy(num,req);
            ////////////////////////
    req.Hunger_Meter+=rand()%10;
    req.Restroom_Meter+=rand()%10;
    if(req.Hunger_Meter>=100&&req.countCafe==0){
        goCafe(num,req);
        req.countCafe+=1;
        req.Hunger_Meter=0;        
    }   
    if(req.Restroom_Meter>=100&&req.countRestroom==0){
        goRestroom(num,req);
        req.Restroom_Meter=0;
        req.countRestroom+=1;
    }
    ////////////////////////
        }
        else{
            printf("Patient %d leaving the hospital after surgery.\n",num);
        }
    }
    
}
void goCafe(int num,Requirement req){

    sem_wait(&cafe);
    printf("Patient %d going to cafe.\n",num);
    randwait(CAFE_TIME);       
    int cafeCost=rand()%CAFE_COST;
    printf("Patient %d paid %d for cafe.\n",num,cafeCost);
    HOSPITAL_WALLET+=cafeCost;    
    sem_post(&cafe);
    printf("Patient %d leaving from cafe.\n",num);
    
}

void goRestroom(int num,Requirement req){
    sem_wait(&restroom);
    printf("Patient %d going to restroom.\n",num);
    randwait(RESTROOM_TIME);    
    sem_post(&restroom);
    printf("Patient %d leaving from restroom.\n",num);
    
}

void goPharmacy(int num,Requirement req){
    sem_wait(&pharmacy);
    printf("Patient %d going to pharmacy.\n",num);
    randwait(PHARMACY_TIME);
    sem_post(&pharmacy);
    int pharcost= rand()%PHARMACY_COST;
    printf("Patient %d paid %d for pharmacy.\n",num,pharcost);
    HOSPITAL_WALLET+=pharcost;
    printf("Patient %d leaving  the hospital.\n",num);
}

void goBloodlab(int num,Requirement req){
    sem_wait(&bloodLab);
    printf("Patient %d going to bloodlab.\n",num);
    randwait(BLOOD_LAB_TIME);
    sem_post(&bloodLab);
    printf("Patient %d paid %d for bloodlab.\n",num,BLOOD_LAB_COST);
    HOSPITAL_WALLET+=BLOOD_LAB_COST;
    printf("Patient %d returning to GP room.\n",num);
    sem_wait(&gpRoom);
    printf("Patient %d entered GP room.\n",num);
    randwait(GP_TIME);
    sem_post(&gpRoom);
}

void goOperation(int num,Requirement req){
    
    int surgeon = rand()%SURGEON_LIMIT;
    int nurse= rand()%NURSE_LIMIT;
    int enS=0;
    int enN=0;
    int a =0;
    while(enS==0||enN==0){//patient waits to assign enough nurse and surgeon
        
        
        if(copysurgeon>surgeon){//assign enough surgeon
            
            copysurgeon-=surgeon;
            enS+=1;
        }   
        if(copynurse>nurse){//assign enough nurse
            copynurse-=nurse;
            enN+=1;
        }
        sleep(WAIT_TIME); 
            ////////////////////////
        req.Hunger_Meter+=rand()%10;
        req.Restroom_Meter+=rand()%10;
        if(req.Hunger_Meter>=100&&req.countCafe==0){
        goCafe(num,req);
        req.countCafe+=1;
        req.Hunger_Meter=0;        
    }   
    if(req.Restroom_Meter>=100&&req.countRestroom==0){
        goRestroom(num,req);
        req.Restroom_Meter=0;
        req.countRestroom+=1;
    }
            //////////////////////// 
        
    }
    sem_wait(&opRoom);    
    printf("Patient %d starting to OP procedure.\n",num);
    randwait(SURGERY_TIME); 
    int opCost=(surgeon*SURGERY_SURGEON_COST)+(nurse*SURGERY_NURSE_COST)+SURGERY_OR_COST;   
    HOSPITAL_WALLET+=opCost;
    printf("Patient %d paid %d for operation.\n",num,opCost);  
    sem_post(&opRoom);  
    sem_wait(&gpRoom);
    printf("Patient %d returning to GP after operation.\n",num);
    
    copysurgeon+=surgeon;
    copynurse+=nurse;//each surgeon and nurse marked as empty
    sem_post(&gpRoom);
}
void randwait(int secs){
    int r = rand() % secs;
    sleep(r/1000);
}

