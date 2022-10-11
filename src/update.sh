touch $1.h
cp $1.ui inter.ui
sed -i 's/\"/\\\"/g' inter.ui
sed 's/$/\\n/' inter.ui | tr -d '\n' > new
rm inter.ui
malloc=$(expr $(du -b new | cut -f -1) + 1)
echo -n 'int initialize_' > $1.h
echo -n $1 >> $1.h
echo '(GtkBuilder* builder) { ' >> $1.h
echo -n '    gtk_builder_add_from_string(builder, "' >> $1.h
cat new >> $1.h
rm new
echo -n '", ' >> $1.h
echo -n $malloc >> $1.h
echo ', NULL);' >> $1.h
echo '}' >> $1.h
