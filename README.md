# Homework 4: Examining a Query Optimizer
### CS186, UC Berkeley, Fall 2012

##Table Of Contents
*  [Outline](https://github.com/cs186-fa12/fa12/blob/master/hw4/README.md#outline)
*  [Environment Setup](https://github.com/cs186-fa12/fa12/blob/master/hw4/README.md#environment-setup)
*  [Questions](https://github.com/cs186-fa12/fa12/blob/master/hw4/README.md#questions)
*  [Submission](https://github.com/cs186-fa12/fa12/blob/master/hw4/README.md#submission)

## Outline
In this homework you will carry out a number of exercises involving the optimization of relational queries using the PostgreSQL optimizer and the visualization command EXPLAIN. You will not have to write any code. **This assignment has to be done individually.**

If you know how to use Postgres, this assignment should be doable in a single sitting.

There are four parts to this assignment:

1. Reading some PostgreSQL documentation
2. Setting up the database, loading the tables, creating indexes
3. Answering questions by observing the query plans generated by PostgreSQL
4. Writing and running some queries of your own.

##Environment Setup

###Virtual Machine
You should use the same virtual machine as in the previous project.

If you would prefer to use an instructional machine, you can do so by logging into one of the hive{1..29}.cs.berkeley.edu machines. Be sure to pay attention to the special instructional machines notes in the setup instructions below.


###Getting the Code
If you are in a new folder, you will have to clone the fa12 repo:

    cd
    git clone git://github.com/cs186-fa12/fa12.git

or if you already had the repo from previous homeworks, you can pull the repo to get the updates:

    cd fa12
    git pull

Make sure you see data in `$HOME/fa12/hw4/`.

###Building Postgres
We will rebuild Postgres so we're not dependant on your buffer manager code working perfectly. Execute the following in the `$HOME/fa12/hw4/postgres-8.4.2` folder.

    ./configure --enable-depend --enable-cassert --enable-debug --prefix=$HOME/pgsql
    make
    make install
    rm -rf $HOME/pgsql/data # this removes your hw3 database!
    $HOME/pgsql/bin/initdb -D $HOME/pgsql/data

**Instructional machines note:** If you're using an instructional machine, add the "--without-readline" option to the `./configure` command. That is, run

    ./configure --enable-depend --enable-cassert --enable-debug --prefix=$HOME/pgsql --without-readline

instead of the first line above.

### Loading Data

Start the postmaster with `pg_ctl`.  Create a database with `createdb`. Create tables and import data into the database. We are creating three tables: 'rankings' and 'financials' and 'students' 

    $HOME/pgsql/bin/pg_ctl -D ~/pgsql/data -o "-p 11111" -l hw4.log start
    $HOME/pgsql/bin/createdb -p 11111 colleges 
    $HOME/pgsql/bin/psql -p 11111 -f ../hw4.sql colleges

**Instructional machines note**: If you're using an instructional machine, you may get an error that says something like "port 11111" is already in use (<em>check hw4.log!!!!</em>). If you do, this means another student is using that number, so you'll want to change the port to something else by replacing the "-p 11111" command with some other number, throughout all commands here and below.

Connect to the `colleges` database using the psql command-line client.

    $HOME/pgsql/bin/psql -p 11111 colleges

Once psql connects to the database, disable bitmap scan access methods. Do not re-enable this access method throughout this assignment.

    SET enable_bitmapscan TO off;

Note: **You MUST update the statistics once before starting the exercises.** The command for updating the statistics (in a psql session) is:

    VACUUM ANALYZE;

**You must also update the PostgreSQL statistics after adding or deleting indexes.** This step is crucial to getting correct answers to the questions!

After completing the above steps you should be able to analyze query plans produced by the optimizer for a given query (using EXPLAIN). You should also be able to modify the runtime variables for enabling/disabling join and access methods (using SET).

## Postgres Documentation
Postgres documentation is available at http://www.postgresql.org/docs/8.2/static/index.html. You will find the following parts particularly useful for this assignment.

<table>
<tr><td>Topic</td><td>Summary</td><td>Documentation</td></tr>
<tr><td>`EXPLAIN` command</td><td>Explains how postgres will process a particular query</td><td><a href='http://www.postgresql.org/docs/8.2/static/sql-explain.html'>Link</a></td></tr>
<tr><td>`CREATE INDEX` command</td><td>Creates an index on a certain column. <br />You will also need `DROP INDEX`.</td><td><a href='http://www.postgresql.org/docs/8.2/static/sql-createindex.html'>Link</a></td></tr>
<tr><td>`SET` command</td><td>Allows you to turn on and off operators that the query optimizer can consider. For example, the SQL command<br/>
    "SET enable_hashjoin TO off;"<br/>
will tell the query optimizer not to use hash joins.</td><td><a href='http://www.postgresql.org/docs/8.2/static/sql-set.html'>Link</a></td></tr>
<tr><td>Runtime query config</td><td>Lists which operators you can enable and disable in the above `SET` command.</td><td><a href='http://www.postgresql.org/docs/8.2/static/runtime-config-query.html#RUNTIME-CONFIG-QUERY-ENABLE'>Link</a></td></tr>
<tr><td>System catalogs</td><td>Details the metadata postgres holds on each table.<br />Note that this information is contained in tables, which can be queried just like any other table. The tables that will be most useful for this assignment are `pg_stats` and `pg_indexes`.</td><td><a href='http://www.postgresql.org/docs/8.2/static/catalogs.html'>Link</a></td></tr>
</table>

## Questions

The GSIs have created a question file in XML format for you to fill in. You should include your answers in hw4.xml. Edit it using your favorite editor, e.g.:

    emacs hw4.xml

For your convenience, the questions are also listed here. Please be aware of the following:
 * Put your answers in blocks wrapped by "<![CDATA[" and "]]>". Each block is marked with TODO. Remove the TODO after putting your answer in.
 * If the "autograde" attribute is "yes" for a question, the question will be marked using a grading script. Make sure you follow the instructions on the format for the answers.
 * Before submitting the assignment, open the XML file in a browser (Mozilla FireFox or Google Chrome) and make sure it passes the validation test (i.e. no error reported).

### Question 1: Some Easy Stuff
Connect to the database.  Type "\d" to see the list of relations and to look at the structure of each relation. Use SQL to look at the data within a relation. Answer the following questions:

A. What are the attributes of the Students relation?

B. How many indexes are built on the Rankings relation? Name them and write down their type.

C. How many tuples are there in the Rankings relation?

D. How many distinct values of "state" does the query planner estimate there are for the Rankings relation? (hint: query pg_stats to find out)

E. How many distinct values of "state" are there actually in the rankings table? (hint: it's probably easiest to run a query to compute this!)

F. What query did you use to find the answer to E?

<a href="http://i.imgur.com/0DhxD.jpg">Man, you're so good at this!</a>

### Question 2: Using the Query Plan Viewer
This question requires you to use the PostgreSQL query plan visualization command `EXPLAIN`. Read the documentation for `EXPLAIN` at the link given above.  Note that (as in class) `EXPLAIN` estimates query costs in units of disk I/Os (CPU instructions are added in by multiplying times a conversion factor).

Consider the following query:

    SELECT * FROM rankings WHERE state = 'CA'; 

Answer the following questions looking at the query plan generated by the `EXPLAIN` command:

A. Briefly describe the plan chosen. (e.g., what kind of scan is used?).

B. In what order would the tuples be returned by this plan? Why?

C. What is the estimated total cost of running the plan?

D. What is the estimated result cardinality for this plan? The estimated result cardinality is the number of colleges that the optimizer estimates to be in California. Looking at the statistics, why does the optimizer come up with this estimate? (hint: it's not simply (# tuples)/(# distinct values) -- examine the pg_stats table more closely to see how postgres could get a better estimate.)

E. How many colleges actually do have "state = 'CA'"?  (hint: it's probably easiest to run a query to compute this!)

F. Looking at the statistics, what are the top 5 states with the most colleges and the percentage of colleges in each of those state? (hint: you can select these by querying on the pg_stats table)

G. Which value of "state" is actually the most popular, and how many college tuples have that "state"? How did you figure this out (what query did you use)?

<a href="http://i.imgur.com/zlpPd.jpg">Explain yourself, dog!</a>

### Question 3: Selects with Indexes
Consider the same query from previous question: 

    SELECT * FROM rankings WHERE state = 'CA';
 
Answer the following questions looking at the plans and the access methods:

A. Create a btree index on the attribute state of the relation Rankings (and `VACUUM ANALYZE;`). What is the plan chosen for the query now? (e.g., what kind of scan is used and what is scanned?).

B. What is the estimated total cost of running the plan?

C. Compare this plan with the plan obtained in question 2.A above.  Which is cheaper and why?

<a href="http://29.media.tumblr.com/tumblr_m0g06fJfyH1qdtbx8o1_500.png">Indexes are tasty!!</a>

### Question 4: Range Selects
`DROP` the index that you created for Question 3. Don't forget to `VACUUM ANALYZE;`

Now analyze the query plan that PostgreSQL comes up for the following query: 

    SELECT * FROM rankings WHERE gradrate < 10;

Answer the following questions:

A. How many ranking tuples that have `gradrate < 10` does the optimizer think there are?

B. In what order will the tuples be returned by this plan?

C. What is **a** value of the constant (i.e. '10' in the above query) such that the optimizer chooses a different plan? What is that plan and why does the optimizer think it will be cheaper than the previous plan when used with this new constant?

<a href="http://i.imgur.com/EKKpO.jpg">I, for one, will definitely graduate.</a>

### Question 5: Simple Join
Create a btree index on 'studentfacultyratio' of the rankings table (and `VACUUM ANALYZE;`). Analyze the query plan for the following query that finds the average salary at schools who have a student to faculty ratio < 5.

    SELECT R.name, F.avesalary 
    FROM rankings R, financials F
    WHERE R.id = F.id AND R.studentfacultyratio < 5; 

Answer the following questions:

A. What is the estimated cost of this plan?

B. What kind of join is used by the plan?

C. Disable the join type used in the above plan and re-analyze the query.  What type of join is used now, and what is the total estimated cost of the query?

D. What relations are sorted in this plan, and why?

<a href="http://26.media.tumblr.com/tumblr_lh4b52TFGl1qd3lqno1_400.jpg">I cannot come up with a caption for this picture.</a>

### Question 6: Three-Way Join
Drop the index that you created for Question 5, RE-ENABLE (using `SET`) the join method you turned off above.

Don't forget to `VACUUM ANALYZE;`

Answer the following questions referring to the query below: 

    SELECT S.name, R.name, F.avesalary
    FROM students S, rankings R, financials F
    WHERE S.school = R.id and R.id = F.id;

A. Describe the best plan estimated by the optimizer. List the joins and access methods it uses, and the order in which the relations are joined.

B. Modify the query by adding a condition R.studentfacultyratio < 8. What is the relational algebra expression for the new join order? Why is this new join ordering better for the extended query than the ordering obtained in part A?

<a href="http://i.imgur.com/oIaDW.jpg">Rawr! I got this!</a>

### Question 7: Playing with SQL
Answer the following questions about the database (by writing queries!). Between the two queries, you will need to use all three relations.

A. What is the name of the public school (public = 1) with the highest average salary?  (hint: while one way to get close to this is list all schools and the average salary for them sorted by average salary, you may also want to try writing a variant that only produces a single result row without using an (expensive) sort - it's a bit ugly).

B. Find the public school (public = 1) with the largest difference between instate tuition and out of state tuition that has at least one student attending.

<a href="http://i.imgur.com/eKnCX.jpg?1">You're done! Go play!</a>

## Submission
Use

    submit hw4

to submit the assignment on an instructional machine. You just need to submit `hw4.xml`.

Before submitting the assignment, run `xmllint --noout hw4.xml` or open the XML file in a browser (Mozilla Firefox or Google Chrome) and make sure it passes the validation test (i.e. no error reported).
=======

optimizer_practice
==================
This projects gives you an in depth understanding of indexing structure in database queries!
