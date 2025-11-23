AegisCAD – AI Rule Definitions
==============================

This folder contains declarative rule sets and configuration data
used by AegisAIEngine and AegisReverseEngine for reasoning,
design optimization, and system behavior.

Each .rule or .json file in this directory defines an expert domain:

1. materials.rules
   - Material safety limits
   - Recommended substitutes
   - Density-to-strength ratios

2. geometry.rules
   - Primitive recognition (box, cylinder, hull, turret, wing)
   - Dimensional proportion checks
   - Edge continuity and symmetry heuristics

3. optimization.rules
   - Weight reduction logic
   - Topology simplification
   - AI-driven FoS recommendations

4. reverse.rules
   - Keyword ↔ geometry mappings for reverse-engineering
   - Tank / Ship / Aircraft / Vehicle templates

5. motion.rules
   - Revolute and prismatic constraint defaults
   - Trace-curve and motion export parameters

The rules are parsed locally by AegisAIEngine at startup and cached
into memory for runtime evaluation. Each rule set is designed to be
extendable without recompiling the application.

