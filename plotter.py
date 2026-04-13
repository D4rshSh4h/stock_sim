#TODO need to make plotter
import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV file
df = pd.read_csv('trade_log.csv')
print(df.columns)

# Extract the Price column
prices = df[' Price']

# Create x-axis (row indices)
x = range(len(prices))

# Plot
plt.plot(x, prices)

# Labels and title
plt.xlabel('Row Number')
plt.ylabel('Price')
plt.title('Price per Row')

# Show graph
plt.show()
print("done")