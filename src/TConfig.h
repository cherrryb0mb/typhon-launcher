#include <map>
#include <string>
#include <tinyxml.h>
#include <SFML/Graphics.hpp>
// Geflissentlich durchdokumentierter ClassHeader !!!

// Dummy Config Klasse, von der alle anderen Klassen typ und id erben.
class Config {
	public:
	std::string typ;
	int id;
};

// Template Voodoo um beliebige Klassen T in einem TConfig Objekt kapseln zu können
template <class T> class TConfig : public Config {
	public:
	
	TConfig() {			// Konstruktor! 
		t=new T();		// alloziiert Speicher für ein T Objekt auf dem Heap
		id=-1;			// und setzt die Default ID auf -1 (wird default nicht in die XML geschrieben)
	}	
	
	TConfig(T element) {	// Nochn Konstruktor um gleich ein T Objekt einzuwickeln
		t=new T(element);	// spart ein späteres set ... bin faul ..
		id=-1;
	}

	 ~TConfig()
	{
		delete t;
	}
	
	T* get() {			// Wichtigstes Ding: Getter-Methode um das eingewickelte Objekt auszuwickeln
		return t;
	}
	
	void set(T element) {	// Zweitwichtigstes Ding: Setter-Methode um .. na, na ..
		t=new T(element);	// .. richtig um T Objekte einzuwickeln !
	}
	
	T operator+(const T& i) {
		*t=*t+i;
		return *t;
	}
	
	T operator-(const T& i) {
		*t=*t-i;
		return *t;
	}



	private:				// Privater Pointer auf das eingewickelte T-Objekt
	T* t;					// Zugriff nur über set und get möglich. Sauberer so ...
};


// Meine Damen und Herren,
// darf ich Ihnen nun die grandiose, einzigartige TyphonConfig Klasse präsentieren:
class TyphonConfig : public Config {
	public:
	~TyphonConfig();
	TyphonConfig();												// mit einem Konstruktor
	
	void set(std::string key, Config* value);					// vielen, vielen überladene Setter-Methoden, 
																// die aber alle die erste Benutzen um Sachen in die Map reinzutun
	
	void set(std::string key, int value);						// tut ein Integer in die Map
	void set(std::string key, std::string value);				// tut nen String in die Map
	void set(std::string key, float value);
	void setBool(std::string key, bool value);
	void setBoolC(std::string key, bool value);
	void set(std::string key, int value, int id);				// tut ein Integer mit ner ID in die Map
	void set(std::string key, std::string value, int id);		// tut nen String mit ID in die Map
	void set(std::string key, float value, int id);
	void setBool(std::string key, bool value, int id);
	
	void set(int key, float value);
	void set(int key, int value);
	void setBool(int key, bool value);
	
	void setCa(std::string key, std::string value);				// tut nen String mit automatischer ID in die Map
	void setCa(std::string key, int value);				// tut nen String mit automatischer ID in die Map

	void setC(std::string key, std::string value);				// tut nen String mit automatischer ID in die Map
	void setC(std::string key, int value);						// tut ein Integer mit automatischer ID in die Map
	void setC(std::string key, float value);		
	
	Config* get(std::string key);								// Chef Getter-Methode, die alle anderen Getter benutzen
	Config* get(int key);
	
	void erase(std::string key);								
	void erase(int id);
	
	void switchid(int id1, int id2);
	void move(int id1, int id2);
	
	void insert(std::string key, Config* value, int id);
	void insert(std::string key, std::string value, int id);
	void insert(std::string key, int value, int id);
	
	TyphonConfig* insertSubCa(std::string key,int id2);
		
	sf::Texture* getTexturePtr(std::string key);
	sf::Shader* getShaderPtr(std::string key);
	int* getIntP(std::string key);								// holt nen Integer-Pointer via Key aus der Map
	int getInt(std::string key);								// holt ein Integer via Key aus der Map
	char* getInt2String(std::string key);						// holt ein Integer via Key aus der Map und castet den zu nem char* um
	char* getFloat2String(std::string key);						// holt ein float via Key aus der Map und castet den zu nem char* um

	std::string* getStringP(std::string key);					// holt nen Pointer zu nem String aus der Map
	std::string getString(std::string key);						// holt nen String aus der Map;
	float getFloat(std::string key);
	float* getFloatP(std::string key);
	bool getBool(std::string key);
	bool* getBoolP(std::string key);
	
	std::string getKey(int id);									// holt zu einer ID den passenden Key
	int size();													// zählt alle ID Elemente einer (sub)Map
	
	sf::Texture* getTexturePtr(int i);
	sf::Shader* getShaderPtr(int i);
	std::string getString(int i);								// holt einen String via ID aus der Map
	std::string* getStringP(int i);								// holt nen Pointer zu nem String via ID aus der Map
	int getInt(int i);											// holt ein Integer via ID aus der Map
	int* getIntP(int i);										// holt nen Pointer zu einem Integer via ID aus der Map
	
	float getFloat(int i);
	float* getFloatP(int i);

	bool getBool(int i);
	bool* getBoolP(int i);
	
	bool exists(std::string key);								// prüft, ob ein Key in der Map vorhanden ist
	bool exists(int id);

	void addConfig(std::string key);							// tut eine Submap erstellen, geht aber auch automatisch
	void addConfig(std::string key, int id);
	
	TyphonConfig* sub(std::string key);							// holt ne Submap via Key aus der map (und erstellt diese, falls nichtexistent)
	TyphonConfig* sub(std::string key, int id);
	TyphonConfig* sub(int i);
	TyphonConfig* subC(std::string key);
	TyphonConfig* subCa(std::string key);
	TyphonConfig* lastsub();

	TiXmlElement* toXML(std::string root);						// Rekursive Funktion um die TyphonConfig zu nem TinyXML Objekt zu machen
	void parseXML(TyphonConfig* tc, TiXmlElement* pElem);		// Rekursive Funktion um ein TinyXML Objekt zu einer TyphonConfig zu machen
	
	void saveXML(std::string filename);							// Speichert die TyphonConfig in eine XML Datei
	void loadXML(std::string filename);							// Lädt die TyphonConfig aus einer XML Datei, überschreibt bestehende Werte, fügt neue hinzu
	void replaceXML(std::string filename);						// Löscht bestehende Config und lädt TyphonConfig aus einer XML Datei
	
	void wipe();												// löscht eine Submap.
	
	template<class T> T addto(std::string key, T value);
	
	private:
	std::map<std::string, Config*>* conf;						// privater Pointer auf die Hauptmap, privat weil sauberer und so.. 
};
