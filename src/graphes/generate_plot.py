import pandas as pd
import matplotlib.pyplot as plt
 
# (fichier csv, titre du graphique, fichier image)
PROGRAMMES = [
    ("../part1/philosophes.csv", "Problème des philosophes", "philosophes.png"),
    ("../part1/producer_consumer.csv", "Producteurs / consommateurs", "producer_consumer.png"),
    ("../part1/reader_writer.csv", "Lecteurs / écrivains", "reader_writer.png"),
]
 
for csv_file, titre, image in PROGRAMMES:
    # Une ligne par configuration : threads, run1, ..., run5
    data = pd.read_csv(csv_file)
 
    threads = data["threads"]
    mesures = data.drop(columns="threads")
 
    mean = mesures.mean(axis=1)
    std = mesures.std(axis=1)  # écart type de l'échantillon (5 mesures)
 
    plt.figure()
    plt.errorbar(threads, mean, yerr=std, fmt="o-", capsize=4,
                 label="Moyenne ± écart type")
    plt.xlabel("Nombre total de threads")
    plt.ylabel("Temps d'exécution (s)")
    plt.title(f"{titre} : temps d'exécution vs nombre de threads")
    plt.xscale("log", base=2)
    plt.xticks(threads, labels=threads)
    plt.ylim(bottom=0)  # l'axe y commence à 0
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.savefig(image, dpi=150)
    plt.close()
    print(f"{image} généré")
 