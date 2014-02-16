#ifndef CANNOTATIONS_H
#define CANNOTATIONS_H

#include "CPlugin.h"
#include "math.h"

//annotation structure
struct teramanager::annotation
{
    int ID;                     //unique identifier
    int type;			//-1 = undefined, 0 = LocationSimple, 1 = NeuronSWC
    int subtype;                //see Vaa3D LocationSimple and NeuronSWC types
    float x, y, z;		//point coordinates
    float r;			//radius
    std::string name;           //name
    std::string comment;        //comment
    RGBA8 color;                //color
    annotation* prev;           //previous annotation handle (used in case of linked structures)
    annotation* next;           //next annotation handle (used in case of linked structures)
    void* container;            //address of the container object

    annotation() throw (RuntimeException){
        type = subtype  = teramanager::undefined_int32;
        r = x = y = z = teramanager::undefined_real32;
        prev = next = 0;
        name = comment = "";
        color.r = color.g = color.b = color.a = 0;
        container = 0;
        if(!recyclableIDs.empty())
        {
            ID = recyclableIDs.front();
            recyclableIDs.pop_front();
        }
        else if(!availableIDs.empty())
        {
            ID = availableIDs.front();
            availableIDs.pop_front();
        }
        else
        {
            char errMsg[STATIC_STRING_SIZE];
            sprintf(errMsg, "in annotation(): no more IDs available. Possible reasons are too many annotations inseted (maximum is %d) or a memory leak."
                    "\n\nPlease signal this bug to the developers.", MAX_ANNOTATIONS_NUMBER);
            throw RuntimeException(errMsg);
        }
    }
    ~annotation(){
        recyclableIDs.push_back(ID);
    }

    static std::list<int> availableIDs;     //list of available IDs, i.e. IDs that can be assigned and that were never assigned before
    static std::list<int> recyclableIDs;    //list of recyclable IDs, i.e. IDs that can be assigned after their owner has been destroyed
};

class teramanager::CAnnotations
{

    private:

        //octree structure
        class Octree
        {
            private:

                struct octant
                {
                    //VHD intervals that delimit the octant
                    itm::uint32 V_start, V_dim;
                    itm::uint32 H_start, H_dim;
                    itm::uint32 D_start, D_dim;

                    //number of neurons in the octant
                    itm::uint32 n_annotations;

                    //annotations
                    std::list<teramanager::annotation*> annotations;

                    //pointers to children octants
                    octant *child1;	//[V_start,         V_start+V_dim/2),[H_start,		H_start+H_dim/2),[D_start,		D_start+D_dim/2)
                    octant *child2;	//[V_start,         V_start+V_dim/2),[H_start,		H_start+H_dim/2),[D_start+D_dim/2,	D_start+D_dim  )
                    octant *child3;	//[V_start,         V_start+V_dim/2),[H_start+H_dim/2,	H_start+H_dim  ),[D_start,		D_start+D_dim/2)
                    octant *child4;	//[V_start,         V_start+V_dim/2),[H_start+H_dim/2,	H_start+H_dim  ),[D_start+D_dim/2,	D_start+D_dim  )
                    octant *child5;	//[V_start+V_dim/2, V_start+V_dim  ),[H_start,		H_start+H_dim/2),[D_start,		D_start+D_dim/2)
                    octant *child6;	//[V_start+V_dim/2, V_start+V_dim  ),[H_start,		H_start+H_dim/2),[D_start+D_dim/2,      D_start+D_dim  )
                    octant *child7;	//[V_start+V_dim/2, V_start+V_dim  ),[H_start+H_dim/2,	H_start+H_dim  ),[D_start,		D_start+D_dim/2)
                    octant *child8;	//[V_start+V_dim/2, V_start+V_dim  ),[H_start+H_dim/2,	H_start+H_dim  ),[D_start+D_dim/2,	D_start+D_dim  )

                    octant(itm::uint32 _V_start, itm::uint32 _V_dim, itm::uint32 _H_start, itm::uint32 _H_dim, itm::uint32 _D_start, itm::uint32 _D_dim)
                    {
                            child1  = child2 = child3 = child4 = child5 = child6 = child7 = child8 = NULL;
                            V_start = _V_start;
                            V_dim   = _V_dim;
                            H_start = _H_start;
                            H_dim   = _H_dim;
                            D_start = _D_start;
                            D_dim   = _D_dim;
                            n_annotations = 0;
                    }
                };
                typedef octant* Poctant;

                itm::uint32 DIM_V, DIM_H, DIM_D;		//volume dimensions (in voxels) along VHD axes
                octant *root;				//pointer to root octant
                Octree(void){}				//default constructor is not available

                /*** SUPPORT methods ***/

                //recursive support methods
                void     _rec_clear(const Poctant& p_octant) throw(RuntimeException);
                void     _rec_insert(const Poctant& p_octant, annotation& neuron) throw(RuntimeException);
                itm::uint32   _rec_deep_count(const Poctant& p_octant) throw(RuntimeException);
                itm::uint32   _rec_height(const Poctant& p_octant) throw(RuntimeException);
                void     _rec_print(const Poctant& p_octant);
                void     _rec_search(const Poctant& p_octant, const interval_t& V_int, const interval_t& H_int, const interval_t& D_int, std::list<annotation*>& neurons) throw(RuntimeException);
                Poctant  _rec_find(const Poctant& p_octant, const interval_t& V_int, const interval_t& H_int, const interval_t& D_int) throw(RuntimeException);
                itm::uint32   _rec_count(const Poctant& p_octant, const interval_t& V_int, const interval_t& H_int, const interval_t& D_int) throw(RuntimeException);

                //returns true if two given volumes intersect each other
                bool inline intersects(const interval_t& V1_int,const interval_t& H1_int,const interval_t& D1_int,
                                       itm::uint32& V2_start, itm::uint32& V2_dim, itm::uint32& H2_start, itm::uint32& H2_dim, itm::uint32& D2_start, itm::uint32& D2_dim)  throw(RuntimeException);

                //returns true if first volume contains second volume
                bool inline contains  (const interval_t& V1_int, const interval_t& H1_int, const interval_t& D1_int,
                                       itm::uint32& V2_start, itm::uint32& V2_dim, itm::uint32& H2_start, itm::uint32& H2_dim, itm::uint32& D2_start, itm::uint32& D2_dim)  throw(RuntimeException);

            public:

                //CONSTRUCTOR and DECONSTRUCTOR
                Octree(itm::uint32 _DIM_V, itm::uint32 _DIM_H, itm::uint32 _DIM_D);
                ~Octree(void);

                //clears octree content and deallocates used memory
                void clear() throw(RuntimeException);

                //insert given neuron in the octree
                void insert(annotation& neuron) throw(RuntimeException);

                //search for neurons in the given 3D volume and puts found neurons into 'neurons'
                void find(interval_t V_int, interval_t H_int, interval_t D_int, std::list<annotation*>& neurons) throw(RuntimeException);

                //search for the annotations at the given coordinate. If found, returns the address of the annotations list
                std::list<annotation*>* find(float x, float y, float z) throw(RuntimeException);

                //returns the number of neurons (=leafs) in the given volume without exploring the entire data structure
                itm::uint32 count(interval_t V_int = interval_t(-1,-1), interval_t H_int = interval_t(-1,-1), interval_t D_int = interval_t(-1,-1))  throw(RuntimeException);

                //returns the number of neurons (=leafs) in the octree by exploring the entire data structure
                itm::uint32 deep_count()  throw(RuntimeException);

                //returns the octree height
                itm::uint32 height()  throw(RuntimeException);

                //print the octree content
                void print();

                friend class CAnnotations;

                static inline double round(double val){
                    return floor(val + 0.5);
                }
                static inline double round(float val){
                    return floor(val + 0.5f);
                }
        };


        //members
        Octree* octree;         //octree associated to the 3D image space where annotations are stored
        int octreeDimX;
        int octreeDimY;
        int octreeDimZ;

        /*********************************************************************************
        * Singleton design pattern: this class can have one instance only,  which must be
        * instantiated by calling static method "istance(...)"
        **********************************************************************************/
        CAnnotations() : octree(0), octreeDimX(undefined_int32), octreeDimY(undefined_int32), octreeDimZ(undefined_int32){}
        static CAnnotations* uniqueInstance;
        CAnnotations(itm::uint32 volHeight, itm::uint32 volWidth, itm::uint32 volDepth) : octreeDimX(volWidth), octreeDimY(volHeight), octreeDimZ(volDepth)
        {
            /**/itm::debug(itm::LEV1, strprintf("volHeight = %d, volWidth = %d, volDepth = %d", volHeight, volWidth, volDepth).c_str(), __itm__current__function__);

            octree = new Octree(octreeDimY, octreeDimX, octreeDimZ);

            /**/itm::debug(itm::LEV1, "object successfully constructed", __itm__current__function__);
        }

    public:

        /*********************************************************************************
        * Singleton design pattern: this class can have one instance only,  which must be
        * instantiated by calling static method "istance(...)"
        **********************************************************************************/
        static CAnnotations* instance(itm::uint32 volHeight, itm::uint32 volWidth, itm::uint32 volDepth)
        {
            if (uniqueInstance == 0)
            {
                for(int i=0; i<teramanager::MAX_ANNOTATIONS_NUMBER; i++)
                    annotation::availableIDs.push_back(i);
                uniqueInstance = new CAnnotations(volHeight, volWidth, volDepth);
            }
            return uniqueInstance;
        }
        static CAnnotations* getInstance() throw (RuntimeException)
        {
            if(uniqueInstance)
                return uniqueInstance;
            else
                throw RuntimeException("in CAnnotations::getInstance(): no object has been instantiated yet");
        }
        static void uninstance();
        ~CAnnotations();

        /*********************************************************************************
        * Adds/removes the given annotation(s)
        **********************************************************************************/
        void addLandmarks(LandmarkList* markers) throw (RuntimeException);
        void removeLandmarks(std::list<LocationSimple> &markers) throw (RuntimeException);
        void addCurves(NeuronTree* curves) throw (RuntimeException);
        void removeCurves(std::list<NeuronSWC> &curves) throw (RuntimeException);

        /*********************************************************************************
        * Retrieves the annotation(s) in the given volume space
        **********************************************************************************/
        void findLandmarks(interval_t X_range, interval_t Y_range, interval_t Z_range, std::list<LocationSimple> &markers) throw (RuntimeException);
        void findCurves(interval_t X_range, interval_t Y_range, interval_t Z_range, std::list<NeuronSWC> &curves) throw (RuntimeException);

        /*********************************************************************************
        * Save/load method
        **********************************************************************************/
        void save(const char* filepath) throw (RuntimeException);
        void load(const char* filepath) throw (RuntimeException);

        /*********************************************************************************
        * Removes all the annotations from the octree
        **********************************************************************************/
        void clear()  throw (RuntimeException)
        {
            /**/itm::debug(itm::LEV1, 0, __itm__current__function__);

            delete octree;
            octree = new Octree(octreeDimY, octreeDimX, octreeDimZ);
        }


};

#endif // CANNOTATIONS_H
