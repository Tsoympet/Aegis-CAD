"""Example: query the AI engine for optimization advice."""
from aegiscad import ai

prompt = "Reduce mass while keeping stiffness"
advice = ai.optimize(prompt)

print("AI summary:\n", advice.summary)
print("Recommendations:")
for item in advice.recommendations:
    print(" -", item)
