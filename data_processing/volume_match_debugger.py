import csv

total = 0

with open("trade_log.csv", "r") as file:
    reader = csv.DictReader(file)
    
    for row in reader:
        total += int(row[" Qty"]) 

print("Total Volume:", total)