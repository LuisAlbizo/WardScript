
range := func { i, e : func : i:
		if i == e then
			nonlocal i := nil;
		else
			nonlocal i := i + 1;
		end
	end};

g := range(0, 8);

present(g);

loop
	i := g();
	present(i);
	if !i then
		exit;
	end
end

