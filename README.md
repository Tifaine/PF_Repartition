Programme Plateforme pour SMA

L'intérêt de ce logiciel est de répartir le travail des agents enregistrés en terme de watching et d'organizing de pattern. 

La plateforme commence par s'enregistrer auprès d'un serveur dont l'adresse est passée en paramètre puis reçoit son nom unique dans le réseau. 
Elle attend ensuite qu'un agent la contacte et vienne s'enregistrer en communiquant son nombre de patterns à gérer et le nombre de pattern qu'ils peuvent analyser.

La plateforme va ensuite organiser chaque agents afin qu'ils instancient des tâches de watching et organizing en fonction du travail à réaliser.



## Le récupérer:
```Shell
> git clone --recurse-submodules https://github.com/Tifaine/PF_Repartition
```

## Compiler:
### Première foi:
```Shell
> ./Configure/configure
> make
```

### Après:
```Sehll
> make
```

## Utilisation:
Vous pouvez lancer le programme comme suit :
```Shell
> ./bin/exec
```

## Options:
Il existe plusieurs options disponibles :
```Shell
> --address (-a) AdresseIpDuServeur
```