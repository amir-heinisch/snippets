# Physical Process Simulation

This code tries to run attacks on process simulations.

1. Create venv and install requirements:

> ``` python -m venv <venv_name> ```
> ``` pip install --upgrade pip setuptools wheel ```
> ``` pip install -r requirements.txt ```

2. Enter venv:

> ``` . <venv_name>/bin/activate ```

3. Run some example:

> ``` python src/run.py ./examples/teRunner.ini ```

4. See the results in ui:

> ``` python src/ui/app.py 127.0.0.1 5000 out/webui.pid out/teRunner.txt ```

or if you just want to see a special variable:

> ``` python src/ui/app.py 127.0.0.1 5000 out/webui.pid out/teRunner.txt <var_nr> ```

