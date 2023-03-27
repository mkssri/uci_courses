This document serves three purposes:
1. It is a guide to setup your account in scharklab.ics.uci.edu
2. It is a guide to create the project's repo (only one member of the team needs to create it).
3. It is a template for the `README.md` file of your repository. Note that all projects will have a similar structure in their `README.md`.

#### Let's get started!

You should have got access to the gitlab repository by now. There are three steps that you need to complete before proceeding:
- **Confirm your email:** Search for an email from noreply@scharklab.ics.uci.edu with subject **"Confirmation instructions"** and follow the link to confirm your email. If you don't find the email, check the spam folder.
- **Set a new password:** Search for an email from noreply@scharklab.ics.uci.edu with subject **"Account was created for you"**, and follow the link to set your new password. This is a NEW password and is unrelated to your UCI account, feel free to create a new one. If you don't find the email, check the spam folder.
- **Setup your SSH keys**: In order to clone/pull/push code to your repo, you need to [create a SSH key](https://scharklab.ics.uci.edu/-/profile/keys). This is setup only once per machine (if you have several computers, and you are planning to work from each of them, then you need to repeat the process in those machines too). If you get lost, read the "Learn more" link in the same page.

#### Create the project's repo

1. **Fork this repo**
    - Only **one member** of the team must fork this repo.
    - Go to https://scharklab.ics.uci.edu/parraca/mat-mult
    - Click the button "Fork" (top right corner).
    - Change the name and slug. If you want, change also the description.
    - Select your own Name for the "namespace" field.
    - **Set the Visibility Level to PRIVATE**.
    - Hit the `Fork project` button.

2. **Add members to the new repo**
    - On your freshly forked repository website, go to: Left Sidebar > Project Information > Members > Invite members (green button at the right top corner)
    - Add the users `isaac` (professor), `arturo` (TA Arturo), and `parraca` (TA Claudio).
    - Add the members of your team with the role of `Maintainer` or `Developer`, so they can write. Find them by their UCI net ID.
    - Scores will be shared among repository members. **NO STUDENT CAN BE MEMBER OF TWO TEAMS**

3. **Start Working on the repo**
    - Clone your new repository into your machine (let's say the username of the repo creator is `ptanteater`, and the repo is called `p1_matmult`):
        ```
        $ git clone git@scharklab.ics.uci.edu:ptanteater/p1_matmult.git
        $ cd p1_matmult
        ```
    - Create a `.gitignore` file, so you don't upload binary stuff to your repo. The content of the `.gitignore` file may be:

        ```
        # object files
        **/*.o

        # backup files
        **/*~
        **/#*
        ```
    - If you want, modify this `README.md` file to remove what is not needed anymore (maybe these instructions).
    - Now `stage` (or `add`, it is the same) the new files, commit, and push your first commit to the server:

        ```
        git stage README.md .gitignore
        git commit -m "Project kick-off"
        git push origin main
        ```

4. **Share the URL**
    - Send the URL of the repo to your teammates, so, after they create their own ssh keys, they can clone/pull/push it to/from their machines. That's it!

----

$`\textcolor{red}{\text{Below, the template for your \texttt{README.md} file. You should remove the content above, including this line, before the submission date.}}`$


# Project 1: Matrix Multiplication

Use this as a template to get started with your project. Your README.md file should contain **three** sections:

- Implementations: description of what you did, and how to use it.
- Metrics and Tests: how did you measure and compare your implementations.
- Findings and Interpretation: What did you find, and what does this mean.

## Implementations
First, give a short summary of the multipliers here implemented, perhaps just a list with a short description for each item. Keep it at a high level, hinting at what are the techniques or key concepts that motivated its inclusion. For example:

- **Naive implementation**: Straight forward implementation of the pen-and-paper algorithm. This is used as a baseline to compare with other implementations.

Then, dedicate one sub-section to each implementation. Use the name of the implementation as the title of the subsection. For example, the one below is the implementation called "FOO-BAR"

### FOO-BAR

#### Theoretical concept
What is the theoretical idea that motivates this implementation. Why it should help in boosting the performance. Focus in the two avenues:
- Efficient use of multiple processing units (parallelism).
- Efficient use of memory hierarchy (cache friendly).

#### Code organization
Here a description in detail of the implementation of FOO-BARR. What is the entry point of your code, what are the source codes involved, what are the data structures used and how does the code work. Get more technical here, so I am able to go to your code and understand what it actually does. (Remember to put some comments in the code itself). Markdown is quite nice in the sense that you can quote pieces of code, include pictures, etc. Use those tools.

#### How to run it
Explain how to build and run the benchmarks of this implementation. If you have a `Makefile` that does the trick, I will instantly love you. Explain here how to use it. If you need me to `cd` into a particular directory to run it, also say it.

Make sure that your instructions are followable: clone your repo in a different machine, and try to follow your own instructions.


## Metrics and tests
In this project, we are evaluating three things (these are the **what** we are measuring):

- Total execution time.
- Good use of multiple cores.
- Good use of cache memory system.

Use this sub-section to describe:

- **How** are you defining "Good use of multiple cores".
- **How** are you defining "Good use of cache memory system".
- **What** are the experiments/benchmarks that you ran in order to measure the metrics described above.
- What environment did you use to run your experiments.
- What tests did you run to verify that the actual multiplication is correct.


## Findings and Interpretations
- What were your findings.
- How can they be interpreted.
- What is the take-away of this project.
- What did you learn.
- What can be done in the future.
