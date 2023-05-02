# Langue

Tous les fichiers sont nommés en **anglais**, préférablement (avec une orthographe US).

# Nommage

## Structure et nommage des fichiers

**Fichiers `.cpp` et `.h`**

Les déclarations et définitions sont séparés entre les fichiers `.h` et `.cpp`.

Les constantes partagées aux autres modules sont mis dans les fichiers `.h`.

On se permettra de faire de potentiels `#include` au sein de fichiers `.h`.

**Classes/Structures**

Chaque Script doit préférablement contenir au maximum 1 **Classe** ou 1 **Structure**, et le fichier doit préférablement avoir le nom de la **Classe** ou de **Structure**.

Si le fichier contient plusieurs parties (plusieurs **Structures** et/ou **Classes**), et qu’ils ne sont pas séparables, le nom de fichier est le nom de la **Classe**/**Structure** la plus importante du fichier (critère subjectif, à l’appréciation de la personne nommant le fichier).

**Enums**

Les **Enums** peuvent être dans le même fichier que des **Classes/Structures** et dans ce cas-là, le fichier aura le nom de la **Classe/Structure** la plus importante.

## Nom des Classes/Structures/Enums

Le nommage en `PascalCase` sera privilégié.

## Fonctions

Le nommage en `PascalCase` sera privilégié. Les noms de fonction commencent préférablement par un verbe.  
Cela peut ne pas être le cas, lorsque les opérations sont des algorithmes précis (ex : `void AStar(...)`).

## Variables

### Variables et paramètres de fonctions

Le nommage en `camelCase` sera privilégié :

```csharp
public void Print(string name, string name2, string otherName)
{
	string functionVariable = name + name2;
}
```

### Attributs
**`private` ou `protected`**

Le nommage se fait en `camelCase` précédé d'un `_` (ex : `_myVariable`).

**`public`**

Le nommage se fait en `PascalCase`.

## Constantes

Les constantes (`#define` ou `const`) sont toutes en majuscules, les mots sont séparés par des `_` :

- `PI`
- `NB_VALEURS`

## Variables particulières
### Pointeurs
Le nom des pointeurs se termine par `_p`, et le `*` des pointeurs est collé au nom de la variable lors de la déclaration.

Les règles précédentes de nommage restent inchangées.
```cpp
class MyClass
{
	private:
		int *_myInt_p;
	public :
		float *MyFloat_p;
	
	float* MyFunction(float *myParam_p);
};
```

### Références
Le nom des références se termine par `_r`, et le `&` des références est collé au nom de la variable lors de la déclaration.

Les règles précédentes de nommage restent inchangées.
```cpp
class MyClass
{
	private:
		int &_myInt_r;
	public :
		float &MyFloat_r;
	
	float MyFunction(float &myParam_r);
};
```

### Fonctions (std::function)

Les nom des variables étant des fonctions se termine par `_f`, et les significations des paramètres seront explicités en commentaire.

Par exemple :
```cpp
void MyFunction
(
	std::function<bool(std::string, std::string)> predicate_f // (firstString, secondString) -> stringsDoMatch 
)
{
	...
}
```  

# Commentaires

## Dans les fichiers `.h`

Les commentaires seront structurés de la manière suivante :  
```cpp
class MyClass
{
	private:
		/*
        =========================================
            Attributes
        =========================================
        */
	   	
	   	// >> Category 1 (Can separate elements in categories if needed)
		
		// Price of the individual object
		int _cost;
		
		// >> Values (it's Category 2)
		
		//
		int _values;
		//
		int &_otherValues_r;
		
		/*
        =========================================
            Methods
        =========================================
        */
	   
		// Does an action
		float DoSomething(int paramOne, float paramTwo);
	
	public:
		/*
        =========================================
            Attributes
        =========================================
        */

		// identifier used for displaying information
		string Name;

		/*
        =========================================
            Methods
        =========================================
        */

		// Does another action which is more complex
		void DoSomethingElse();
};
```

## Dans les fichiers `.cpp`

Il sera préférable de recopier les description des fonctions dans les fichiers `.cpp`.

Des commentaires sont ajoutés au sein des fonctions/méthodes pour décrire leur déroulement, notamment si elles sont complexes à comprendre.

```cpp
// Does an action
float MyClass::DoSomething(int paramOne, float paramTwo) 
{
	float sum = paramOne + paramTwo;
	return sum;
} 

// Does another action which is more complex
void MyClass::DoSomethingElse()
{ 
	// Cost should not exceed this maximum value, 
	// because it is important for our program
	if (_cost > 1000)
		return;

	// The longer the name is, the pricier it gets
	// This operation has a long comment because it is quite complex
	_cost += Name.length();
}
```

